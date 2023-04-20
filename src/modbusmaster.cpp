#include "modbusmaster.h"

ModbusMaster::ModbusMaster(ModbusType type) { modbus_type_ = type; }

Modbus::ModbusReplyStatus ModbusMaster::masterDataProcess(unsigned char *recv_data, int recv_len, int start_addr, ModbusErrorCode *err_code)
{
    return kModbusSuccess;
}

///
/// \brief ModbusMaster::masterReadCoils            主站侧读输入线圈 01
/// \param start_addr
/// \param read_len
/// \param send_buff
/// \return
///
int ModbusMaster::masterReadCoils(int slave_id, int start_addr, int read_len,
                                unsigned char* send_buff) {
  int i = 0;
  send_buff[i++] = slave_id;
  send_buff[i++] = kReadCoils;
  send_buff[i++] = (start_addr >> 8) & 0xff;
  send_buff[i++] = (start_addr)&0xff;
  send_buff[i++] = (read_len >> 8) & 0xff;
  send_buff[i++] = (read_len)&0xff;

  return addHeaderAndTailMessage(send_buff, i);
}

///
/// \brief ModbusMaster::masterReadDiscreteInputs   主站侧读离散输入 02
/// \param start_addr
/// \param read_len
/// \param send_buff
/// \return
///
int ModbusMaster::masterReadDiscreteInputs(int slave_id, int start_addr,
                                         int read_len,
                                         unsigned char* send_buff) {
  int i = 0;
  send_buff[i++] = slave_id;
  send_buff[i++] = kReadDiscreteInputs;
  send_buff[i++] = (start_addr >> 8) & 0xff;
  send_buff[i++] = (start_addr)&0xff;
  send_buff[i++] = (read_len >> 8) & 0xff;
  send_buff[i++] = (read_len)&0xff;

  return addHeaderAndTailMessage(send_buff, i);
}

///
/// \brief ModbusMaster::masterReadHoldRegs     主站侧读保持寄存器  03
/// \param start_addr
/// \param read_len
/// \param send_buff
/// \return
///
int ModbusMaster::masterReadHoldRegs(int slave_id, int start_addr, int read_len,
                                   unsigned char* send_buff) {
  int i = 0;
  send_buff[i++] = slave_id;
  send_buff[i++] = kReadHoldingRegisters;
  send_buff[i++] = (start_addr >> 8) & 0xff;
  send_buff[i++] = (start_addr)&0xff;
  send_buff[i++] = (read_len >> 8) & 0xff;
  send_buff[i++] = (read_len)&0xff;

  return addHeaderAndTailMessage(send_buff, i);
}

///
/// \brief ModbusMaster::masterReadInputRegisters            主站侧读输入寄存器 04
/// \param start_addr
/// \param read_len
/// \param send_buff
/// \return
///
int ModbusMaster::masterReadInputRegisters(int slave_id, int start_addr,
                                         int read_len,
                                         unsigned char* send_buff) {
  int i = 0;
  send_buff[i++] = slave_id;
  send_buff[i++] = kReadInputRegisters;
  send_buff[i++] = (start_addr >> 8) & 0xff;
  send_buff[i++] = (start_addr)&0xff;
  send_buff[i++] = (read_len >> 8) & 0xff;
  send_buff[i++] = (read_len)&0xff;

  return addHeaderAndTailMessage(send_buff, i);
}

///
/// \brief ModbusMaster::masterWriteSingleCoil      主站侧写单个线圈 05
/// \param start_addr
/// \param value
/// \return
///
int ModbusMaster::masterWriteSingleCoil(int modbus_id, int start_addr, int value,
                                      unsigned char* send_buff) {
  int i = 0;
  send_buff[i++] = modbus_id;
  send_buff[i++] = kWriteSingleCoil;
  send_buff[i++] = (start_addr >> 8) & 0xff;
  send_buff[i++] = (start_addr)&0xff;
  send_buff[i++] = (value >> 8) & 0xff;
  send_buff[i++] = (value)&0xff;

  return addHeaderAndTailMessage(send_buff, i);
}

///
/// \brief ModbusMaster::masterWriteSingleRegister  主站侧写单个寄存器 06
/// \param start_addr
/// \param value
/// \return
///
int ModbusMaster::masterWriteSingleRegister(int slave_id, int start_addr,
                                          int value, unsigned char* send_buff) {
  int i = 0;
  send_buff[i++] = slave_id;
  send_buff[i++] = kWriteSingleRegister;
  send_buff[i++] = (start_addr >> 8) & 0xff;
  send_buff[i++] = (start_addr)&0xff;
  send_buff[i++] = (value >> 8) & 0xff;
  send_buff[i++] = (value)&0xff;

  return addHeaderAndTailMessage(send_buff, i);
}

///
/// \brief ModbusMaster::masterWriteCoils   主机侧写多个线圈  0f
/// \param start_addr
/// \param len
/// \param value_data
/// \param send_buff
/// \return
///
int ModbusMaster::masterWriteCoils(int slave_id, int start_addr, int len,
                                 unsigned char* value_data,
                                 unsigned char* send_buff) {
  int i = 0;
  send_buff[i++] = slave_id;
  send_buff[i++] = kWriteMultipleCoils;
  send_buff[i++] = (start_addr >> 8) & 0xff;
  send_buff[i++] = (start_addr)&0xff;
  send_buff[i++] = (len >> 8) & 0xff;
  send_buff[i++] = (len)&0xff;
  send_buff[i++] = (len % 8) == 0 ? (len / 8) : (len / 8 + 1);
  for (int j = 0; j < len; j++) {
    send_buff[i + j / 8] |= (value_data[j] << (j & 8));
    i++;
  }
  return addHeaderAndTailMessage(send_buff, i);
}

///
/// \brief ModbusMaster::masterWriteRegisters       主站侧写多个寄存器 10
/// \param start_addr
/// \param len
/// \param value_data
/// \param send_buff
/// \return
///
int ModbusMaster::masterWriteRegisters(int slave_id, int start_addr, int len,
                                     unsigned char* value_data,
                                     unsigned char* send_buff) {
  int i = 0;
  send_buff[i++] = slave_id;
  send_buff[i++] = kWriteMultipleRegisters;
  send_buff[i++] = (start_addr >> 8) & 0xff;
  send_buff[i++] = (start_addr)&0xff;
  send_buff[i++] = (len >> 8) & 0xff;
  send_buff[i++] = (len)&0xff;
  send_buff[i++] = len * 2;
  for (int j = 0; j < len; j++) {
    send_buff[i++] = (value_data[j] >> 8) & 0xff;
    send_buff[i++] = (value_data[j]) & 0xff;
  }
  return addHeaderAndTailMessage(send_buff, i);
}

void ModbusMaster::masterCoilsUpdate(int slave_id, int addr, int16_t value)
{

}

void ModbusMaster::masterHoldRegUpdate(int slave_id, int addr, int16_t value)
{

}

void ModbusMaster::masterDiscreteInputUpdate(int slave_id, int addr, int16_t value)
{

}

void ModbusMaster::masterInputRegistersUpdate(int slave_id, int addr, int16_t value)
{

}
