#ifndef MODBUSMASTER_H
#define MODBUSMASTER_H

#include "modbus.h"

class ModbusMaster : public Modbus {
public:
    ModbusMaster(ModbusType type);
    ///
    /// \brief masterDataProcess      modbus主站侧数据处理
    /// \param recv_data            从站返回的数据
    /// \param recv_len             从站返回数据长度
    /// \param start_addr           主站请求从站的地址
    /// \param err_code             从站回复错误码
    /// \return         数据校验结果
    ///
    ModbusReplyStatus masterDataProcess(uint8_t* recv_data, int recv_len,ModbusErrorCode* err_code);

    ///
    /// \brief ModbusBase::masterReadCoils            主站侧读输入线圈 01
    /// \param start_addr
    /// \param read_len
    /// \return 发送数据长度
    ///
    int masterReadCoils(int slave_id, int start_addr, int read_len);

    ///
    /// \brief ModbusBase::masterReadDiscreteInputs   主站侧读离散输入 02
    /// \param start_addr
    /// \param read_len
    /// \return 发送数据长度
    ///
    int masterReadDiscreteInputs(int slave_id, int start_addr, int read_len);

    ///
    /// \brief ModbusBase::masterReadHoldRegs     主站侧读保持寄存器  03
    /// \param start_addr
    /// \param read_len
    /// \return 发送数据长度
    ///
    int masterReadHoldRegs(int slave_id, int start_addr, int read_len);

    ///
    /// \brief ModbusBase::masterReadInputRegisters            主站侧读输入寄存器 04
    /// \param start_addr
    /// \param read_len
    /// \return 发送数据长度
    ///
    int masterReadInputRegisters(int slave_id, int start_addr, int read_len);

    ///
    /// \brief ModbusBase::masterWriteSingleCoil      主站侧写单个线圈 05
    /// \param start_addr
    /// \param value
    /// \return 发送数据长度
    ///
    int masterWriteSingleCoil(int slave_id, int start_addr, int value);

    ///
    /// \brief ModbusBase::masterWriteSingleRegister  主站侧写单个寄存器 06
    /// \param start_addr
    /// \param value
    /// \return   发送数据长度
    ///
    int masterWriteSingleRegister(int slave_id, int start_addr, int value);

    ///
    /// \brief ModbusBase::masterWriteCoils   主机侧写多个线圈  0f
    /// \param start_addr
    /// \param len
    /// \param value_data
    /// \return   发送数据长度
    ///
    int masterWriteCoils(int slave_id, int start_addr, int len,
                         uint8_t* value_data);

    ///
    /// \brief ModbusBase::masterWriteRegisters       主站侧写多个寄存器 10
    /// \param start_addr
    /// \param len
    /// \param value_data
    /// \return  发送数据长度
    ///
    int masterWriteRegisters(int slave_id, int start_addr, int len,
                             uint8_t* value_data);

public:
    virtual int32_t SendData(uint8_t * data,int32_t len);

public:
    virtual void masterCoilsUpdate(int slave_id, int addr,int16_t value);
    virtual void masterHoldRegUpdate(int slave_id, int addr,int16_t value);
    virtual void masterDiscreteInputUpdate(int slave_id, int addr,int16_t value);
    virtual void masterInputRegistersUpdate(int slave_id, int addr,int16_t value);

private:
    struct{
        uint32_t slave_id;
        uint32_t addr;
        uint32_t funcode;
        uint32_t len;
    }op;
};

#endif  // MODBUSMASTER_H
