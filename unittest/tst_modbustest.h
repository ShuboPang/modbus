#ifndef TST_MODBUSTEST_H
#define TST_MODBUSTEST_H

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include<initializer_list>
#include "modbusslavetest.h"
#include "modbusmastertest.h"


using namespace testing;

uint8_t recv_buff[MODBUS_MAX_PRIVATE_BUFFER_LEN]={0};
uint8_t send_buff[MODBUS_MAX_PRIVATE_BUFFER_LEN]={0};

uint8_t recv_len = 0;
uint8_t send_len = 0;


///
/// \brief DataInit     数据初始化，每个测试用例最开始都要执行初始化
///
void DataInit(){
    memset(recv_buff,0,sizeof (recv_buff));
    memset(send_buff,0,sizeof (send_buff));
    recv_len = 0;
    send_len = 0;
}


TEST(ModbusTest, ModbusTest)
{
    EXPECT_EQ(1, 1);
    ASSERT_THAT(0, Eq(0));
}

///
/// \brief TEST  测试 Crc错误的帧是否能正常解析
///
TEST(ModbusSlaveUnitTest, ModbusSlaveUnitTest_CrcError)
{
    DataInit();
    ModbusSlaveTest* test = new ModbusSlaveTest(ModbusSlaveTest::kModbusRtu);

    //< 错误的校验和测试
    recv_len = ModbusSlaveTest::SetBuffData(recv_buff,{0x01,0x03,0x00,0x01,0x00,0x01,0x00,0x00});
    test->SetModbusID(1);
    ModbusSlave::ModbusReplyStatus status = test->slaveDataProcess(recv_buff,recv_len,send_buff,&send_len);
    EXPECT_EQ(status, ModbusSlave::kModbusDataError);

    delete test;
}

///
/// \brief TEST  测试正常modbus帧是否能正常解析
///
TEST(ModbusSlaveUnitTest, ModbusSlaveUnitTest_CrcOk)
{
    DataInit();
    ModbusSlaveTest* test = new ModbusSlaveTest(ModbusSlaveTest::kModbusRtu);

    recv_len = ModbusSlaveTest::SetBuffData(recv_buff,{0x01,0x03,0x00,0x01,0x00,0x01,0xca,0xd5});
    test->SetModbusID(1);
    ModbusSlave::ModbusReplyStatus status = test->slaveDataProcess(recv_buff,recv_len,send_buff,&send_len);
    EXPECT_EQ(status, ModbusSlave::kModbusSuccess);

    delete test;
}

///
/// \brief TEST     测试 03 功能码读寄存器
///
TEST(ModbusSlaveUnitTest, ModbusSlaveUnitTest_ReadHoldRegs)
{
    DataInit();
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

///
/// \brief TEST
///
/// 1. 设置从站ID 为1  从站保持寄存器地址1中写123
/// 2. 主站读从站1 地址123 保持寄存器值
/// 3. 从站返回数据交给主站处理
/// 4. 判断主站读回的值是否为123
///
TEST(ModbusMasterUnitTest, ModbusMasterUnitTest_ReadHoldRegs)
{
    uint16_t slave_id = 1;
    uint16_t read_addr = 0x01;
    int16_t value = 123;

    DataInit();

    ModbusSlaveTest* slave = new ModbusSlaveTest(ModbusSlaveTest::kModbusRtu);
    slave->SetModbusID(slave_id);

    ModbusMasterTest * master = new ModbusMasterTest(ModbusMasterTest::kModbusRtu);

    recv_len = ModbusSlaveTest::SetBuffData(recv_buff,
               {0x01,0x03,static_cast<unsigned char>((read_addr>>8)&0x0ff),static_cast<unsigned char>((read_addr&0xff)),0x00,0x01,0xca,0xd5});



    // 主站读 从站1 地址1 长度1
    master->masterReadHoldRegs(1,1,1);

    EXPECT_EQ(recv_len,8);
    for(uint32_t i = 0;i<recv_len;i++){
        EXPECT_EQ(master->send_buff[i],recv_buff[i]);
    }

    slave->hold_regs[read_addr] = value;



    // 从站处理主站的数据
    ModbusSlave::ModbusReplyStatus status = slave->slaveDataProcess(recv_buff,recv_len,send_buff,&send_len);
    EXPECT_EQ(status, ModbusSlave::kModbusSuccess);

    // 主站处理从站的数据
    ModbusSlaveTest::ModbusErrorCode error = ModbusSlaveTest::kModbusExceptionNone;
    status = master->masterDataProcess(send_buff,send_len,&error);

    EXPECT_EQ(status, ModbusMasterTest::kModbusSuccess);
    EXPECT_EQ(error, ModbusMasterTest::kModbusExceptionNone);
    EXPECT_EQ(value,master->slave[slave_id][read_addr]);

    delete slave;
    delete master;
}

#endif // TST_MODBUSTEST_H
