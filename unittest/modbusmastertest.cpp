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

void ModbusMasterTest::masterCoilsUpdate(int slave_id, int addr, int16_t value)
{
    slave[slave_id][addr] = value;
}

void ModbusMasterTest::masterHoldRegUpdate(int slave_id, int addr, int16_t value)
{
    slave[slave_id][addr] = value;
}

void ModbusMasterTest::masterDiscreteInputUpdate(int slave_id, int addr, int16_t value)
{
    slave[slave_id][addr] = value;
}

void ModbusMasterTest::masterInputRegistersUpdate(int slave_id, int addr, int16_t value)
{
    slave[slave_id][addr] = value;
}
