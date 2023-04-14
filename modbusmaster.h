#ifndef MODBUSMASTER_H
#define MODBUSMASTER_H

#include "modbus.h"

class ModbusMaster : public Modbus {
 public:
  ModbusMaster();
  ///
  /// \brief hostDataProcess      modbus主站侧数据处理
  /// \param recv_data            从站返回的数据
  /// \param recv_len             从站返回数据长度
  /// \param start_addr           主站请求从站的地址
  /// \param err_code             从站回复错误码
  /// \return         数据校验结果
  ///
  ModbusReplyStatus hostDataProcess(unsigned char* recv_data, int recv_len,
                                    int start_addr, ModbusErrorCode* err_code);

  ///
  /// \brief ModbusBase::hostReadCoils            主站侧读输入线圈 01
  /// \param start_addr
  /// \param read_len
  /// \param send_buff
  /// \return
  ///
  int hostReadCoils(int slave_id, int start_addr, int read_len,
                    unsigned char* send_buff);

  ///
  /// \brief ModbusBase::hostReadDiscreteInputs   主站侧读离散输入 02
  /// \param start_addr
  /// \param read_len
  /// \param send_buff
  /// \return
  ///
  int hostReadDiscreteInputs(int slave_id, int start_addr, int read_len,
                             unsigned char* send_buff);

  ///
  /// \brief ModbusBase::hostReadHoldRegs     主站侧读保持寄存器  03
  /// \param start_addr
  /// \param read_len
  /// \param send_buff
  /// \return
  ///
  int hostReadHoldRegs(int slave_id, int start_addr, int read_len,
                       unsigned char* send_buff);

  ///
  /// \brief ModbusBase::hostReadInputRegisters            主站侧读输入寄存器 04
  /// \param start_addr
  /// \param read_len
  /// \param send_buff
  /// \return
  ///
  int hostReadInputRegisters(int modbus_id, int start_addr, int read_len,
                             unsigned char* send_buff);

  ///
  /// \brief ModbusBase::hostWriteSingleCoil      主站侧写单个线圈 05
  /// \param start_addr
  /// \param value
  /// \return
  ///
  int hostWriteSingleCoil(int slave_id, int start_addr, int value,
                          unsigned char* send_buff);

  ///
  /// \brief ModbusBase::hostWriteSingleRegister  主站侧写单个寄存器 06
  /// \param start_addr
  /// \param value
  /// \return
  ///
  int hostWriteSingleRegister(int slave_id, int start_addr, int value,
                              unsigned char* send_buff);

  ///
  /// \brief ModbusBase::hostWriteCoils   主机侧写多个线圈  0f
  /// \param start_addr
  /// \param len
  /// \param value_data
  /// \param send_buff
  /// \return
  ///
  int hostWriteCoils(int slave_id, int start_addr, int len,
                     unsigned char* value_data, unsigned char* send_buff);

  ///
  /// \brief ModbusBase::hostWriteRegisters       主站侧写多个寄存器 10
  /// \param start_addr
  /// \param len
  /// \param value_data
  /// \param send_buff
  /// \return
  ///
  int hostWriteRegisters(int slave_id, int start_addr, int len,
                         unsigned char* value_data, unsigned char* send_buff);
};

#endif  // MODBUSMASTER_H
