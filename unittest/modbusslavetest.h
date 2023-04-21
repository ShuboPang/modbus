#ifndef MODBUSSLAVETEST_H
#define MODBUSSLAVETEST_H

#include "modbusslave.h"
#include <map>

class ModbusSlaveTest:public ModbusSlave
{
public:
    ModbusSlaveTest(ModbusType type);

    ~ModbusSlaveTest();

    using REGISTER_MAP = std::map<uint16_t,int16_t>;

public:
    // 读保持寄存器
     ModbusErrorCode slaveReadHoldRegHandle(uint16_t addr, uint16_t* value) override;

    // 写保持寄存器
     ModbusErrorCode slaveWriteHoldRegHandle(uint16_t addr, uint16_t value) override;

    // 读线圈
     ModbusErrorCode slaveReadCoilHandle(uint16_t addr, uint16_t* value) override;

    // 读线圈
     ModbusErrorCode slaveWriteCoilHandle(uint16_t addr, uint16_t value) override;

    // 读离散输入
     ModbusErrorCode slaveReadDiscreteInputHandle(uint16_t addr, uint16_t* value) override;

    // 读输入寄存器
     ModbusErrorCode slaveReadInputRegsHandle(uint16_t addr, uint16_t* value) override;

 public:
     REGISTER_MAP hold_regs;
};

#endif // MODBUSSLAVETEST_H
