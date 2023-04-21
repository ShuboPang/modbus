#ifndef TST_MODBUSTEST_H
#define TST_MODBUSTEST_H

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include<initializer_list>
#include "modbusslavetest.h"


using namespace testing;

uint8_t recv_buff[MODBUS_MAX_PRIVATE_BUFFER_LEN]={0};
uint8_t send_buff[MODBUS_MAX_PRIVATE_BUFFER_LEN]={0};

uint8_t recv_len = 0;
uint8_t send_len = 0;




TEST(ModbusTest, ModbusTest)
{
    EXPECT_EQ(1, 1);
    ASSERT_THAT(0, Eq(0));
}

TEST(ModbusSlaveUnitTest, ModbusSlaveUnitTest_CrcError)
{
    ModbusSlaveTest* test = new ModbusSlaveTest(ModbusSlaveTest::kModbusRtu);

    //< 错误的校验和测试
    recv_len = ModbusSlaveTest::SetBuffData(recv_buff,{0x01,0x03,0x00,0x01,0x00,0x01,0x00,0x00});
    test->SetModbusID(1);
    ModbusSlave::ModbusReplyStatus status = test->slaveDataProcess(recv_buff,recv_len,send_buff,&send_len);
    EXPECT_EQ(status, ModbusSlave::kModbusDataError);

    delete test;
}

TEST(ModbusSlaveUnitTest, ModbusSlaveUnitTest_CrcOk)
{
    ModbusSlaveTest* test = new ModbusSlaveTest(ModbusSlaveTest::kModbusRtu);

    recv_len = ModbusSlaveTest::SetBuffData(recv_buff,{0x01,0x03,0x00,0x01,0x00,0x01,0xca,0xd5});
    test->SetModbusID(1);
    ModbusSlave::ModbusReplyStatus status = test->slaveDataProcess(recv_buff,recv_len,send_buff,&send_len);
    EXPECT_EQ(status, ModbusSlave::kModbusSuccess);

    delete test;
}


TEST(ModbusSlaveUnitTest, ModbusSlaveUnitTest_ReadHoldRegs)
{
    ModbusSlaveTest* test = new ModbusSlaveTest(ModbusSlaveTest::kModbusRtu);

    recv_len = ModbusSlaveTest::SetBuffData(recv_buff,{0x01,0x03,0x00,0x01,0x00,0x01,0xca,0xd5});
    test->SetModbusID(1);
    ModbusSlave::ModbusReplyStatus status = test->slaveDataProcess(recv_buff,recv_len,send_buff,&send_len);
    EXPECT_EQ(status, ModbusSlave::kModbusSuccess);
    EXPECT_EQ(send_buff[0],0x01);
    EXPECT_EQ(send_buff[1],0x83);
    EXPECT_EQ(send_buff[2],0x02);

    delete test;
}

#endif // TST_MODBUSTEST_H
