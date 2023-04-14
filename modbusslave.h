#ifndef MODBUSSLAVE_H
#define MODBUSSLAVE_H

#include "modbus.h"

class ModbusSlave : public Modbus {
 public:
  ModbusSlave();
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
  ModbusReplyStatus slaveDataProcess(unsigned char* recv_data, int recv_len,
                                     unsigned char* send_data, int* send_len);

  //// 从机收到主站数据后调用
  // 读保持寄存器
  virtual ModbusErrorCode slaveReadHoldRegHandle(int addr, uint16_t* value);

  // 写保持寄存器
  virtual ModbusErrorCode slaveWriteHoldRegHandle(int addr, int value);

  // 读线圈
  virtual ModbusErrorCode slaveReadCoilHandle(int addr, int* value);

  // 读线圈
  virtual ModbusErrorCode slaveWriteCoilHandle(int addr, int value);

  // 读离散输入
  virtual ModbusErrorCode slaveReadDiscreteInputHandle(int addr, int* value);

  // 读输入寄存器
  virtual ModbusErrorCode slaveReadInputRegsHandle(int addr, int* value);

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
                              uint16_t* recv_data, unsigned char* send_data);

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
                 unsigned char* send_buff, int* send_len);

  ///
  /// \brief errorReply
  /// \param funCode
  /// \param errCode
  /// \param sendBuff
  ///
  void errorReply(int fun_code, int errCode, unsigned char* sendBuff);

 private:
  int modbus_id_;  // 从站设置  modbus id
};

#endif  // MODBUSSLAVE_H
