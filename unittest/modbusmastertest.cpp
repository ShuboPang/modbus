#include "modbusmastertest.h"

ModbusMasterTest::ModbusMasterTest(ModbusType type):ModbusMaster(type)
{

}

int32_t ModbusMasterTest::SendData(uint8_t *data, int32_t len)
{
    memset(send_buff,0,sizeof (send_buff));
    memcpy(send_buff,data,len);

    return 0;
}

void ModbusMasterTest::masterCoilsUpdate(uint16_t slave_id, uint16_t addr, int16_t value)
{

}

void ModbusMasterTest::masterHoldRegUpdate(uint16_t slave_id, uint16_t addr, int16_t value)
{
    slave[slave_id][addr] = value;
}

void ModbusMasterTest::masterDiscreteInputUpdate(uint16_t slave_id, uint16_t addr, int16_t value)
{

}

void ModbusMasterTest::masterInputRegistersUpdate(uint16_t slave_id, uint16_t addr, int16_t value)
{

}
