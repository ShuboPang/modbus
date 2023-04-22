#include "modbusslave.h"
#include "crc.h"



ModbusSlave::ModbusSlave(ModbusType type) { modbus_type_ = type; }

ModbusSlave::ModbusReplyStatus ModbusSlave::slaveDataProcess(
    uint8_t* recv_data, uint8_t recv_len, uint8_t* send_data,
    uint8_t* send_len) {
    int ret = kModbusExceptionNone;
    int realPos = 0;      //< TCP : 6  rtu:0

    // 数据校验  去除头部和尾部
    if (modbus_type_ == kModbusRtu) {
        if (recv_len < MODBUS_RTU_DATA_MIN_LENGTH) {
            return kModbusDataError;
        }
        // Crc校验
        uint16_t crc0 =
            (recv_data[recv_len - 2]) | ((uint16_t)recv_data[recv_len - 1] << 8);
        uint16_t crc1 = Crc::crc16((uint8_t*)recv_data, recv_len - MODBUS_RTU_CRC_LENGTH);

        if (crc1 != crc0) {
            return kModbusDataError;
        }
        recv_len -= 2;
    } else if (modbus_type_ == kModbusTcp) {
        if (recv_len < MODBUS_TCP_DATA_MIN_LENGTH) {
            return kModbusDataError;
        }

        int tmp_len = recv_data[4];         //< modbus数据长度
        tmp_len = tmp_len << 8;
        tmp_len |=  recv_data[5];

        if (tmp_len + MODBUS_RTU_CRC_LENGTH != recv_len) {
            return kModbusDataError;
        }
        // modbus tcp 前6个字节为头部
        realPos = MODBUS_RTU_CRC_LENGTH;
        recv_len -= realPos;
        uint16_t tcp_head_count = recv_data[0];
        tcp_head_count = tcp_head_count << 8;
        tcp_head_count |= recv_data[1];
        serial_number = tcp_head_count;
    }

    uint16_t modbusID = recv_data[realPos + MODBUS_ID_POS];
    uint16_t fun_code = recv_data[realPos + MODBUS_FUNCODE_POS];
    uint16_t addr =
        ((uint16_t)recv_data[realPos + 2] << 8) | recv_data[realPos + 3];
    uint16_t data[MODBUS_MAX_PRIVATE_BUFFER_LEN] = {0};
    uint8_t value_data[MODBUS_MAX_PRIVATE_BUFFER_LEN] = {0};
    if (modbusID != modbus_id_ && modbusID != 0 && modbusID != 254) {
        send_data[0] = modbusID;
        return kModbusIdError;
    }
    int len = 0;
    len = ((uint16_t)recv_data[realPos + 4] << 8) | recv_data[realPos + 5];
    switch (fun_code) {
    case kReadCoils:             // 0x01
    case kReadDiscreteInputs:    // 0x02
    case kReadHoldingRegisters:  // 0x03
    case kReadInputRegisters:    // 0x04
        if (recv_len != 6) {
            // id 功能码 地址2 长度2
            return kModbusDataError;
        }
        break;
    case kWriteSingleCoil:      // -----------------------0x05
    case kWriteSingleRegister:  // 0x06
        if (recv_len != 6) {
            // id 功能码 地址2 长度2
            return kModbusDataError;
        }
        len = 1;
        data[0] = (recv_data[realPos + 4] << 8) | recv_data[realPos + 5];
        break;
    case kWriteMultipleCoils:  // -------------------0x0f--------------------
    {
        int byte_len = recv_data[realPos + 6];
        if ((byte_len + 7) != recv_len) {
            return kModbusDataError;
        }

        for (int i = 0; i < len; i++) {
            data[i] = recv_data[realPos + 7 + i / 8] >> (i % 8);
        }
    } break;
    case kWriteMultipleRegisters:  // -----------------0x10---------------------
        if (recv_len != len * 2 + 7) {
            return kModbusDataError;
        }
        for (int i = 0; i < len; i++) {
            data[i] = (recv_data[realPos + 7 + i * 2] << 8) |
                      recv_data[realPos + 8 + i * 2];
        }
        break;
    default:
        ret = kModbusExceptionIllegalFunction;
        break;
    }
    bool is_custom = false;
    if (ret == kModbusExceptionNone) {
        // 根据功能码，获取地址数据 存储到valueData中
        ret = dataAnalyze(fun_code, addr, len, data, value_data);
    }
    else if(ret == kModbusExceptionIllegalFunction){      //< 自定义功能码解析
        fun_code = recv_data[realPos + 1];
        ret = this->slaveReadCustomHandle(fun_code,&(recv_data[realPos + 2]),recv_len-2,value_data,send_len);
        if(ret != kModbusExceptionIllegalFunction){
            is_custom = true;
        }
    }

    if (ret == kModbusExceptionNone ) {
        if(!is_custom){
            // 将value_data拼接为返回帧
            dataReply(fun_code, addr, len, data, value_data, send_len);
            memcpy(send_data, value_data, *send_len);
        }
        else{
            send_data[0] = (uint8_t)modbus_id_;
            send_data[1] = (uint8_t)fun_code;
            memcpy(send_data+2, value_data, *send_len);
        }
    } else {
        // 拼接错误帧
        errorReply(fun_code, ret, send_data);
        *send_len = 3;
    }

    // 拼接帧头和帧尾
    *send_len = addHeaderAndTailMessage(send_data, *send_len);

    if (modbusID == 0) {
        // 广播地址不回复
        *send_len = 0;
    }

    return kModbusSuccess;
}

// 读保持寄存器
ModbusSlave::ModbusErrorCode ModbusSlave::slaveReadHoldRegHandle(
    uint16_t addr, uint16_t* value) {
    return kModbusExceptionIllegalDataAddress;
}

// 写保持寄存器
ModbusSlave::ModbusErrorCode ModbusSlave::slaveWriteHoldRegHandle(uint16_t addr,
                                                                  uint16_t value) {
    return kModbusExceptionIllegalDataAddress;
}

// 读线圈
ModbusSlave::ModbusErrorCode ModbusSlave::slaveReadCoilHandle(uint16_t addr,
                                                              uint16_t* value) {
    return kModbusExceptionIllegalDataAddress;
}

// 写线圈
ModbusSlave::ModbusErrorCode ModbusSlave::slaveWriteCoilHandle(uint16_t addr,
                                                               uint16_t value) {
    return kModbusExceptionIllegalDataAddress;
}

// 读离散输入
ModbusSlave::ModbusErrorCode ModbusSlave::slaveReadDiscreteInputHandle(
    uint16_t addr, uint16_t* value) {
    return kModbusExceptionIllegalDataAddress;
}

// 读输入寄存器
ModbusSlave::ModbusErrorCode ModbusSlave::slaveReadInputRegsHandle(uint16_t addr,
                                                                   uint16_t* value) {
    return kModbusExceptionIllegalDataAddress;
}

///
/// \brief ModbusSlave::slaveReadCustomHandle
/// \param funcode
/// \param recv_data
/// \param recv_len
/// \param send_data
/// \param send_len
/// \return
///
Modbus::ModbusErrorCode ModbusSlave::slaveReadCustomHandle(uint8_t funcode,const uint8_t *recv_data, int recv_len, uint8_t *send_data, uint8_t *send_len)
{
    return kModbusExceptionIllegalFunction;
}

// Modbus 数据处理函数
//  funCode: 功能码
//  addr:地址
//  len:长度
//  recvData:接收到的数据 16bit 写指令用
//  sendData:需要发送的数据 读指令用 8bit
// return :
//   错误码
ModbusSlave::ModbusErrorCode ModbusSlave::dataAnalyze(
    int funCode, int addr, int len, uint16_t* recv_data,
    uint8_t* send_data) {
    ModbusErrorCode ret = kModbusExceptionNone;
    uint16_t tmpValue = 0;
    switch (funCode) {
    case kReadCoils:  // 0x01
        for (int i = 0; i < len; i++) {
            ret = slaveReadCoilHandle(addr + i, &tmpValue);
            if (ret != kModbusExceptionNone) {
                break;
            }
            send_data[i] = tmpValue;
        }
        break;
    case kReadDiscreteInputs:  // 0x02
        for (int i = 0; i < len; i++) {
            ret = slaveReadDiscreteInputHandle(addr + i, &tmpValue);
            if (ret != kModbusExceptionNone) {
                break;
            }
            send_data[i] = tmpValue;
        }
        break;
    case kReadHoldingRegisters:  // 0x03
    {
        for (uint32_t i = 0; i < len; i++) {
            uint16_t data = 0;
            ret = slaveReadHoldRegHandle(addr + i, &data);
            if (ret != kModbusExceptionNone) {
                //            std::cout << "0x03::ret != kModbusExceptionNone;";
                break;
            }
            send_data[i * 2] = data >> 8;
            send_data[i * 2 + 1] = data & 0xFF;
            //        std::cout << "data[" << i << "]: " << data << std::endl;
            //        PRINT_HEX("hhhh",data,1);
        }
    } break;
    case kReadInputRegisters:  // 0x04
        for (uint32_t i = 0; i < len; i++) {
            uint16_t data = 0;
            ret = slaveReadInputRegsHandle(addr + i, &data);
            if (ret != kModbusExceptionNone) {
                break;
            }
            send_data[i * 2] = data >> 8;
            send_data[i * 2 + 1] = data;
        }
        break;
    case kWriteSingleCoil:  // 0x05
        ret = slaveWriteCoilHandle(addr, recv_data[0]);
        break;
    case kWriteSingleRegister:  // 0x06
        ret = slaveWriteHoldRegHandle(addr, recv_data[0]);
        break;
    case kWriteMultipleCoils:  // 0x0f
        for (int i = 0; i < len; i++) {
            ret = slaveWriteCoilHandle(addr + i, recv_data[i] == 1 ? 1 : 0);
            if (ret != kModbusExceptionNone) {
                break;
            }
        }
        break;
    case kWriteMultipleRegisters:  // 0x10
        for (int i = 0; i < len; i++) {
            ret = slaveWriteHoldRegHandle(addr + i, recv_data[i]);
            if (ret != kModbusExceptionNone) {
                break;
            }
        }
        break;
    default:
        ret = kModbusExceptionIllegalFunction;
        break;
    }
    return ret;
}

void ModbusSlave::dataReply(int fun_code, int addr, int len,
                            uint16_t* recv_data, uint8_t* send_buff,
                            uint8_t* send_len) {
    char buff[MODBUS_MAX_PRIVATE_BUFFER_LEN] = {0};
    buff[0] = (uint8_t)modbus_id_;
    buff[1] = (uint8_t)fun_code;
    switch (fun_code) {
    case kReadCoils:             // 0x01
    case kReadDiscreteInputs:    // 0x02
    case kReadHoldingRegisters:  // 0x03
    case kReadInputRegisters:    // 0x04
        buff[2] = (len * 2) & 0xff;
        for (uint32_t i = 0; i < len * 2; i++) {
            buff[3 + i] = send_buff[i];
        }
        len = len * 2 + 3;
        *send_len = len;
        memcpy(send_buff, buff, len);
        break;
    case kWriteSingleCoil:      // 0x05
    case kWriteSingleRegister:  // 0x06
        buff[2] = (addr >> 8) & 0xff;
        buff[3] = (addr >> 0) & 0xff;
        buff[4] = (recv_data[0] >> 8) & 0xff;
        buff[5] = recv_data[0] & 0xff;
        len = 6;
        memcpy(send_buff, buff, len);
        *send_len = len;
        break;
    case kWriteMultipleCoils:      // 0x0f
    case kWriteMultipleRegisters:  // 0x10
        buff[2] = (addr >> 8) & 0xff;
        buff[3] = (addr >> 0) & 0xff;
        buff[4] = (len >> 8) & 0xff;
        buff[5] = (len >> 0) & 0xff;
        len = 6;
        *send_len = len;
        memcpy(send_buff, buff, len);
        break;
    }
}

///
/// \brief ModbusSlave::errorReply       回复异常错误帧
/// \param fun_code                     功能码
/// \param errCode                      错误代码
/// \param sendBuff
///
void ModbusSlave::errorReply(int fun_code, int errCode,
                             uint8_t* sendBuff) {
    sendBuff[0] = modbus_id_;
    sendBuff[1] = fun_code;
    sendBuff[1] |= 1 << 7;
    sendBuff[2] = errCode;
}
