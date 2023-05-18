#ifndef MODBUSSLAVE_H
#define MODBUSSLAVE_H

#include "modbus.h"

class ModbusSlave : public Modbus {
public:
    ModbusSlave(ModbusType type);
    virtual ~ModbusSlave() {}

public:
    void SetModbusID(int id) { modbus_id_ = id; }

    int GetModbusID(void) { return modbus_id_; }

public:
    ///
    /// \brief slaveDataProcess     modbus 从站侧数据处理
    /// \param recv_data            从站接收到的数据
    /// \param recv_len             从站接收到的数据长度
    /// \param send_data            从站回复数据
    /// \param send_len             从站回复数据
    /// \return     从站接收到的数据是否为modbus数据帧格式
    ///
    ModbusReplyStatus slaveDataProcess(uint8_t* recv_data, uint8_t recv_len,
                                       uint8_t* send_data, uint8_t* send_len);

    //// 从机收到主站数据后调用
    // 读保持寄存器
    virtual ModbusErrorCode slaveReadHoldRegHandle(uint16_t addr, uint16_t* value);

    // 写保持寄存器
    virtual ModbusErrorCode slaveWriteHoldRegHandle(uint16_t addr, uint16_t value);

    // 读线圈
    virtual ModbusErrorCode slaveReadCoilHandle(uint16_t addr, uint16_t* value);

    // 读线圈
    virtual ModbusErrorCode slaveWriteCoilHandle(uint16_t addr, uint16_t value);

    // 读离散输入
    virtual ModbusErrorCode slaveReadDiscreteInputHandle(uint16_t addr, uint16_t* value);

    // 读输入寄存器
    virtual ModbusErrorCode slaveReadInputRegsHandle(uint16_t addr, uint16_t* value);

    ///
    /// \brief slaveReadCustomHandle          自定义操作
    /// \param funcode                        功能码
    /// \param recv_data                      接收到的数据 不包含Modbus ID、功能码 和 校验和
    /// \param recv_len                       上个参数的长度
    /// \param send_data                      需要发送的数据 不包含Modbus ID 、功能码 和 校验和
    /// \param send_len                       发送数据长度
    /// \return
    ///
    virtual ModbusErrorCode slaveReadCustomHandle(uint8_t funcode, const uint8_t* recv_data,int recv_len,uint8_t* send_data,uint8_t* send_len);

private:
    ///
    /// \brief dataAnalyze      数据解析
    /// \param funCode          功能码
    /// \param addr             起始地址
    /// \param len              长度
    /// \param recvData         接收数据
    /// \param sendData         发送数据
    /// \return
    ///
    ModbusErrorCode dataAnalyze(int fun_code, int addr, int len,
                                uint16_t* recv_data, uint8_t* send_data);

    ///
    /// \brief dataReply
    /// \param funCode
    /// \param addr
    /// \param len
    /// \param recvData
    /// \param sendbuff
    /// \param sendLen
    ///
    void dataReply(int fun_code, int addr, int len, uint16_t* recv_data,
                   uint8_t* send_buff, uint8_t *send_len);

    ///
    /// \brief errorReply
    /// \param funCode
    /// \param errCode
    /// \param sendBuff
    ///
    void errorReply(int fun_code, int errCode, uint8_t* sendBuff);

private:
    int modbus_id_;  // 从站设置  modbus id
};

#endif  // MODBUSSLAVE_H
