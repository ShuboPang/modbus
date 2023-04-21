#ifndef MODBUS_H
#define MODBUS_H

#include <stdint.h>
#include <string.h>
#include <iostream>

#define MODBUS_MAX_PRIVATE_BUFFER_LEN 256  // 数据buff最大长度
#define MODBUS_RTU_DATA_MIN_LENGTH      5               //< modbus rtu 最小帧长度
#define MODBUS_TCP_DATA_MIN_LENGTH      9               //< modbus TCP 最小帧长度
#define MODBUS_ID_POS                   0               //< modbus id 在数据中的位置
#define MODBUS_FUNCODE_POS              1               //< modbus funcode 在数据中的位置
#define MODBUS_RTU_CRC_LENGTH           2               //< modbus rtu crc的长度
#define MODBUS_TCP_HEAD                 6               //< modbus tcp 头部长度

#define CPL_BIT(value, bit) (value ^= (1 << bit))   //取反指定位
#define SET_BIT(value, bit) (value |= (1 << bit))   //置位指定位
#define CLR_BIT(value, bit) (value &= ~(1 << bit))  //清零指定位
#define GET_BIT(value, bit) (value & (1 << bit))    //读取指定位

// 打印HEX字节流  参数：备注  字节流指针 长度
#define PRINT_HEX(note, source, len) \
  printByteToHex(note, source, len, __FILE__, __FUNCTION__, __LINE__)

void printByteToHex(const char* note, const unsigned char* source,
                    int source_len, const char* file, const char* function,
                    int line);

class Modbus {
public:
    Modbus();

    virtual ~Modbus() {}

    enum ModbusErrorCode {
        kModbusExceptionNone = 0x00,
        kModbusExceptionIllegalFunction = 0x01,       //< 非法功能码
        kModbusExceptionIllegalDataAddress = 0x02,    //< 非法数据地址
        kModbusExceptionIllegalDataValue = 0x03,      //< 非法数据值
        kModbusExceptionSlaveOrServerFailure = 0x04,  //< 从站设备故障
        kModbusExceptionAcknowledge = 0x05,           //<
        //从站确认请求，但需长时间处理，返回该状态避免主站执行超时处理；
        kModbusExceptionSlaveOrServerBusy =
            0x06,  //< 从站忙（正在执行长时间处理程序）
        kModbusExceptionNegativeAcknowlege,
        kModbusExceptionMemoryParity,  //< 存储奇偶性校验错误 文件处理时用到
        kModbusExceptionNotDefined,
        kModbusExceptionGatewayPath = 0x0A,    //< 不可用网关
        kModbusExceptionGatewayTarget = 0x0B,  //< 目标设备响应失败
        kModbusExceptionMax
    };

        enum ModbusType {
            kModbusAscii = 0,  //
            kModbusRtu = 1,
            kModbusTcp = 2,
            };

    enum ModbusMode {
        kModbusHost,   // modbus主站
        kModbusSlave,  // modbu从站
    };

    enum ModbusFunctionCode {
        kReadCoils = 0x01,               //< 读线圈
        kReadDiscreteInputs = 0x02,      //< 读输入离散量
        kReadHoldingRegisters = 0x03,    //< 读多个寄存器
        kReadInputRegisters = 0X04,      //< 读输入寄存器
        kWriteSingleCoil = 0X05,         //< 写单个线圈
        kWriteSingleRegister = 0X06,     //< 写单个寄存器
        kWriteMultipleCoils = 0X0F,      //< 写多个线圈
        kWriteMultipleRegisters = 0X10,  //< 写多个寄存器
    };

    enum ModbusReplyStatus    // 数据回复状态
    { kModbusSuccess = 0,     // 数据校验成功 正常帧和错误帧
      kModbusIdError = -1,    // 主机id错误
      kModbusDataError = -2,  // 数据错误，不是modbus数据帧格式
      kModbusNotMatch = -3,   //< 数据不匹配 从站返回的数据与主站不对应
      kModbusLengthError = -4,//< 从站回复的数据长度不是2的整数倍
    };

    explicit Modbus(ModbusType type);

public:
    ///
    /// \brief addTcpHeader     为modbus数据帧添加modbus tcp头部
    /// \param ptr
    /// \param len
    /// \return                 返回添加后的数据长度
    ///
    unsigned int addTcpHeader(unsigned char* ptr, uint16_t len);

    ///
    /// \brief addHeaderAndTailMessage      添加modbusTcp头部或modbusRtu校验和
    /// \param ptr                          数据指针
    /// \param len                          数据长度
    /// \return                             添加帧信息后数据长度
    ///
    unsigned int addHeaderAndTailMessage(unsigned char* ptr, unsigned char len);

protected:
    ModbusType modbus_type_;  // 0:modbus ascii 1:modbus rtu  2:modbus tcp
    uint16_t serial_number;      //<      tcp的流水号
};

#endif  // MODBUS_H
