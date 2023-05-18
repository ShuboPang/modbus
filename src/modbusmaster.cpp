#include "modbusmaster.h"
#include "crc.h"

ModbusMaster::ModbusMaster(ModbusType type) { modbus_type_ = type; }

Modbus::ModbusReplyStatus ModbusMaster::masterDataProcess(uint8_t* recv_data, int recv_len,ModbusErrorCode* err_code)
{
    int realPos = 0;      //< TCP : 6  rtu:0
    uint32_t tcp_head_count = 0;
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

        if (tmp_len + MODBUS_TCP_HEAD != recv_len) {
            return kModbusDataError;
        }
        // modbus tcp 前6个字节为头部
        realPos = MODBUS_TCP_HEAD;
        recv_len -= realPos;
        tcp_head_count = recv_data[0];
        tcp_head_count = tcp_head_count << 8;
        tcp_head_count |= recv_data[1];

        if(tcp_head_count  != serial_number){
            return kModbusNotMatch;
        }
    }

    //< 解析Modbus 从站数据
    uint16_t modbusID = recv_data[realPos + MODBUS_ID_POS];
    uint16_t fun_code = recv_data[realPos + MODBUS_FUNCODE_POS];
    uint16_t len = recv_data[realPos + 2];
    uint8_t* data = recv_data + realPos+3;


    //< 从站返回的数据与主站不匹配
    if(modbusID != op.slave_id && op.slave_id != 0 && op.slave_id != 254){
        return kModbusNotMatch;
    }

    if(fun_code != op.funcode){
        uint8_t fun = op.funcode | (1<<7);
        if(fun_code == fun){        //< 从站返回错误帧
            *err_code = (ModbusErrorCode)recv_data[realPos + 2];
            return kModbusSuccess;
        }
        return kModbusNotMatch;
    }

    if(fun_code == kReadCoils){
        for(uint32_t i = 0;i<op.len;i++){
            uint32_t addr = op.addr + i;
            uint8_t value = data[i/8];
            this->masterCoilsUpdate(modbusID,addr,GET_BIT(value,i%8));
        }
    }
    else if(fun_code == kReadDiscreteInputs){
        for(uint32_t i = 0;i<op.len;i++){
            uint32_t addr = op.addr + i;
            uint8_t value = data[i/8];
            this->masterDiscreteInputUpdate(modbusID,addr,GET_BIT(value,i%8));
        }
    }
    else if(fun_code == kReadHoldingRegisters) {
        for(uint32_t i = 0;i<op.len;i++){
            uint32_t addr = op.addr + i;
            int16_t value = data[i*2+0];
            value = value << 8;
            value |= data[i*2+1];
            this->masterHoldRegUpdate(modbusID,addr,value);

        }
    }
    else if(fun_code == kReadInputRegisters) {
        for(uint32_t i = 0;i<op.len;i++){
            uint32_t addr = op.addr + i;
            int16_t value = data[i*2+0];
            value = value << 8;
            value |= data[i*2+1];
            this->masterInputRegistersUpdate(modbusID,addr,value);
        }
    }
    this->serial_number++;
    return kModbusSuccess;
}

///
/// \brief ModbusMaster::masterReadCoils            主站侧读输入线圈 01
/// \param start_addr
/// \param read_len
/// \param send_buff
/// \return
///
int ModbusMaster::masterReadCoils(int slave_id, int start_addr, int read_len) {
    int i = 0;
    uint8_t send_buff[MODBUS_MAX_PRIVATE_BUFFER_LEN]= {0};
    send_buff[i++] = slave_id;
    send_buff[i++] = kReadCoils;
    send_buff[i++] = (start_addr >> 8) & 0xff;
    send_buff[i++] = (start_addr)&0xff;
    send_buff[i++] = (read_len >> 8) & 0xff;
    send_buff[i++] = (read_len)&0xff;
    int len = addHeaderAndTailMessage(send_buff, i);

    op.addr = start_addr;
    op.funcode = kReadCoils;
    op.len = read_len;
    op.slave_id = slave_id;
    this->SendData(send_buff,len);
    return 0;
}

///
/// \brief ModbusMaster::masterReadDiscreteInputs   主站侧读离散输入 02
/// \param start_addr
/// \param read_len
/// \param send_buff
/// \return
///
int ModbusMaster::masterReadDiscreteInputs(int slave_id, int start_addr,
                                           int read_len) {
    int i = 0;
    uint8_t send_buff[MODBUS_MAX_PRIVATE_BUFFER_LEN]= {0};
    send_buff[i++] = slave_id;
    send_buff[i++] = kReadDiscreteInputs;
    send_buff[i++] = (start_addr >> 8) & 0xff;
    send_buff[i++] = (start_addr)&0xff;
    send_buff[i++] = (read_len >> 8) & 0xff;
    send_buff[i++] = (read_len)&0xff;

    int len = addHeaderAndTailMessage(send_buff, i);

    op.addr = start_addr;
    op.funcode = kReadDiscreteInputs;
    op.len = read_len;
    op.slave_id = slave_id;
    this->SendData(send_buff,len);
    return 0;
}

///
/// \brief ModbusMaster::masterReadHoldRegs     主站侧读保持寄存器  03
/// \param start_addr
/// \param read_len
/// \param send_buff
/// \return
///
int ModbusMaster::masterReadHoldRegs(int slave_id, int start_addr, int read_len) {
    int i = 0;
    uint8_t send_buff[MODBUS_MAX_PRIVATE_BUFFER_LEN]= {0};
    send_buff[i++] = slave_id;
    send_buff[i++] = kReadHoldingRegisters;
    send_buff[i++] = (start_addr >> 8) & 0xff;
    send_buff[i++] = (start_addr)&0xff;
    send_buff[i++] = (read_len >> 8) & 0xff;
    send_buff[i++] = (read_len)&0xff;

    int len = addHeaderAndTailMessage(send_buff, i);

    op.addr = start_addr;
    op.funcode = kReadHoldingRegisters;
    op.len = read_len;
    op.slave_id = slave_id;
    this->SendData(send_buff,len);
    return 0;
}

///
/// \brief ModbusMaster::masterReadInputRegisters            主站侧读输入寄存器 04
/// \param start_addr
/// \param read_len
/// \param send_buff
/// \return
///
int ModbusMaster::masterReadInputRegisters(int slave_id, int start_addr,
                                           int read_len) {
    int i = 0;
    uint8_t send_buff[MODBUS_MAX_PRIVATE_BUFFER_LEN]= {0};
    send_buff[i++] = slave_id;
    send_buff[i++] = kReadInputRegisters;
    send_buff[i++] = (start_addr >> 8) & 0xff;
    send_buff[i++] = (start_addr)&0xff;
    send_buff[i++] = (read_len >> 8) & 0xff;
    send_buff[i++] = (read_len)&0xff;

    int len = addHeaderAndTailMessage(send_buff, i);

    op.addr = start_addr;
    op.funcode = kReadInputRegisters;
    op.len = read_len;
    op.slave_id = slave_id;
    this->SendData(send_buff,len);
    return 0;
}

///
/// \brief ModbusMaster::masterWriteSingleCoil      主站侧写单个线圈 05
/// \param start_addr
/// \param value
/// \return
///
int ModbusMaster::masterWriteSingleCoil(int slave_id, int start_addr, int value) {
    int i = 0;
    uint8_t send_buff[MODBUS_MAX_PRIVATE_BUFFER_LEN]= {0};
    send_buff[i++] = slave_id;
    send_buff[i++] = kWriteSingleCoil;
    send_buff[i++] = (start_addr >> 8) & 0xff;
    send_buff[i++] = (start_addr)&0xff;
    send_buff[i++] = (value >> 8) & 0xff;
    send_buff[i++] = (value)&0xff;

    int len = addHeaderAndTailMessage(send_buff, i);

    op.addr = start_addr;
    op.funcode = kWriteSingleCoil;
    op.len = 1;
    op.slave_id = slave_id;
    this->SendData(send_buff,len);
    return 0;
}

///
/// \brief ModbusMaster::masterWriteSingleRegister  主站侧写单个寄存器 06
/// \param start_addr
/// \param value
/// \return
///
int ModbusMaster::masterWriteSingleRegister(int slave_id, int start_addr,
                                            int value) {
    int i = 0;
    uint8_t send_buff[MODBUS_MAX_PRIVATE_BUFFER_LEN]= {0};
    send_buff[i++] = slave_id;
    send_buff[i++] = kWriteSingleRegister;
    send_buff[i++] = (start_addr >> 8) & 0xff;
    send_buff[i++] = (start_addr)&0xff;
    send_buff[i++] = (value >> 8) & 0xff;
    send_buff[i++] = (value)&0xff;

    int len = addHeaderAndTailMessage(send_buff, i);

    op.addr = start_addr;
    op.funcode = kWriteSingleRegister;
    op.len = 1;
    op.slave_id = slave_id;
    this->SendData(send_buff,len);
    return 0;
}

///
/// \brief ModbusMaster::masterWriteCoils   主机侧写多个线圈  0f
/// \param start_addr
/// \param len
/// \param value_data
/// \param send_buff
/// \return
///
int ModbusMaster::masterWriteCoils(int slave_id, int start_addr, int coil_len,
                                   uint8_t* value_data) {
    int i = 0;
    uint8_t send_buff[MODBUS_MAX_PRIVATE_BUFFER_LEN]= {0};
    send_buff[i++] = slave_id;
    send_buff[i++] = kWriteMultipleCoils;
    send_buff[i++] = (start_addr >> 8) & 0xff;
    send_buff[i++] = (start_addr)&0xff;
    send_buff[i++] = (coil_len >> 8) & 0xff;
    send_buff[i++] = (coil_len)&0xff;
    send_buff[i++] = (coil_len % 8) == 0 ? (coil_len / 8) : (coil_len / 8 + 1);
    for (int j = 0; j < coil_len; j++) {
        send_buff[i + j / 8] |= (value_data[j] << (j & 8));
        i++;
    }

    int len = addHeaderAndTailMessage(send_buff, i);

    op.addr = start_addr;
    op.funcode = kWriteMultipleCoils;
    op.len = coil_len;
    op.slave_id = slave_id;
    this->SendData(send_buff,len);
    return 0;
}

///
/// \brief ModbusMaster::masterWriteRegisters       主站侧写多个寄存器 10
/// \param start_addr
/// \param len
/// \param value_data
/// \param send_buff
/// \return
///
int ModbusMaster::masterWriteRegisters(int slave_id, int start_addr, int reg_len,
                                       uint8_t* value_data) {
    int i = 0;
    uint8_t send_buff[MODBUS_MAX_PRIVATE_BUFFER_LEN]= {0};
    send_buff[i++] = slave_id;
    send_buff[i++] = kWriteMultipleRegisters;
    send_buff[i++] = (start_addr >> 8) & 0xff;
    send_buff[i++] = (start_addr)&0xff;
    send_buff[i++] = (reg_len >> 8) & 0xff;
    send_buff[i++] = (reg_len)&0xff;
    send_buff[i++] = reg_len * 2;
    for (int j = 0; j < reg_len; j++) {
        send_buff[i++] = (value_data[j] >> 8) & 0xff;
        send_buff[i++] = (value_data[j]) & 0xff;
    }
    int len = addHeaderAndTailMessage(send_buff, i);

    op.addr = start_addr;
    op.funcode = kWriteMultipleRegisters;
    op.len = reg_len;
    op.slave_id = slave_id;
    this->SendData(send_buff,len);
    return 0;
}

int32_t ModbusMaster::SendData(uint8_t *data, int32_t len)
{
    return 0;
}

void ModbusMaster::masterCoilsUpdate(uint16_t slave_id, uint16_t addr, int16_t value)
{

}

void ModbusMaster::masterHoldRegUpdate(uint16_t slave_id, uint16_t addr, int16_t value)
{

}

void ModbusMaster::masterDiscreteInputUpdate(uint16_t slave_id, uint16_t addr, int16_t value)
{

}

void ModbusMaster::masterInputRegistersUpdate(uint16_t slave_id, uint16_t addr, int16_t value)
{

}
