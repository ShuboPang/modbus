#ifndef MODBUSMASTERTEST_H
#define MODBUSMASTERTEST_H

#include <map>
#include "modbusmaster.h"

class ModbusMasterTest: public ModbusMaster
{
public:
    ModbusMasterTest(ModbusType type);

    using REGISTER_MAP = std::map<uint16_t,int16_t>;
    using SLAVE_REGISTER_MAP = std::map<uint16_t,REGISTER_MAP>;
public:
    int32_t SendData(uint8_t * data,int32_t len) override;

public:
    void masterCoilsUpdate(uint16_t slave_id, uint16_t addr,int16_t value) override;
    void masterHoldRegUpdate(uint16_t slave_id, uint16_t addr,int16_t value) override ;
    void masterDiscreteInputUpdate(uint16_t slave_id, uint16_t addr,int16_t value) override;
    void masterInputRegistersUpdate(uint16_t slave_id, uint16_t addr,int16_t value) override;

public:
    SLAVE_REGISTER_MAP slave;

    uint8_t send_buff[MODBUS_MAX_PRIVATE_BUFFER_LEN]={0};
    uint8_t send_len = 0;

};

#endif // MODBUSMASTERTEST_H
