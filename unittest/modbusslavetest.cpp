#include "modbusslavetest.h"

ModbusSlaveTest::ModbusSlaveTest(ModbusType type):ModbusSlave(type)
{

}

ModbusSlaveTest::~ModbusSlaveTest()
{

}

Modbus::ModbusErrorCode ModbusSlaveTest::slaveReadHoldRegHandle(uint16_t addr, uint16_t *value)
{
    if(hold_regs.find(addr) != hold_regs.end()){
        *value = hold_regs[addr];
        return kModbusExceptionNone;
    }
    return kModbusExceptionIllegalDataAddress;
}

Modbus::ModbusErrorCode ModbusSlaveTest::slaveWriteHoldRegHandle(uint16_t addr, uint16_t value)
{
    hold_regs[addr] = value;
    return kModbusExceptionNone;
}

Modbus::ModbusErrorCode ModbusSlaveTest::slaveReadCoilHandle(uint16_t addr, uint16_t *value)
{
    if(hold_regs.find(addr) != hold_regs.end()){
        *value = hold_regs[addr];
        return kModbusExceptionNone;
    }
    return kModbusExceptionNone;
}

Modbus::ModbusErrorCode ModbusSlaveTest::slaveWriteCoilHandle(uint16_t addr, uint16_t value)
{
    hold_regs[addr] = value;
    return kModbusExceptionNone;
}

Modbus::ModbusErrorCode ModbusSlaveTest::slaveReadDiscreteInputHandle(uint16_t addr, uint16_t *value)
{
    return kModbusExceptionNone;
}

Modbus::ModbusErrorCode ModbusSlaveTest::slaveReadInputRegsHandle(uint16_t addr, uint16_t *value)
{
    return kModbusExceptionNone;
}
