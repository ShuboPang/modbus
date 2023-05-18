#ifndef TST_MODBUSTEST_H
#define TST_MODBUSTEST_H

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include<initializer_list>
#include "modbusslavetest.h"
#include "modbusmastertest.h"
#include "crc.h"


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
/// \brief TEST   测试数据长度不够的情况
///
TEST(ModbusSlaveUnitTest, ModbusSlaveUnitTest_Buff)
{
    DataInit();
    ModbusSlaveTest* test = new ModbusSlaveTest(ModbusSlaveTest::kModbusRtu);

    //< 错误的校验和测试
    recv_len = ModbusSlaveTest::SetBuffData(recv_buff,{0x01,0x03,0x00});
    test->SetModbusID(1);
    ModbusSlave::ModbusReplyStatus status = test->slaveDataProcess(recv_buff,recv_len,send_buff,&send_len);
    EXPECT_EQ(status, ModbusSlave::kModbusDataError);

    delete test;
}

TEST(ModbusSlaveTcpUnitTest, ModbusSlaveUnitTest_Buff)
{
    DataInit();
    ModbusSlaveTest* test = new ModbusSlaveTest(ModbusSlaveTest::kModbusTcp);

    //< 错误的校验和测试
    recv_len = ModbusSlaveTest::SetBuffData(recv_buff,{0x01,0x03,0x00});
    test->SetModbusID(1);
    ModbusSlave::ModbusReplyStatus status = test->slaveDataProcess(recv_buff,recv_len,send_buff,&send_len);
    EXPECT_EQ(status, ModbusSlave::kModbusDataError);

    delete test;
}

TEST(ModbusSlaveTcpUnitTest, ModbusSlaveUnitTest_BuffOk)
{
    DataInit();
    ModbusSlaveTest* test = new ModbusSlaveTest(ModbusSlaveTest::kModbusTcp);

    //< 错误的校验和测试
    recv_len = ModbusSlaveTest::SetBuffData(recv_buff,{0x00,0x03,0x00,0x00,0x00,0x06,0x01,0x03,0x00,0x01,0x00,0x01});
    test->SetModbusID(1);
    ModbusSlave::ModbusReplyStatus status = test->slaveDataProcess(recv_buff,recv_len,send_buff,&send_len);
    EXPECT_EQ(status, ModbusSlave::kModbusSuccess);

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
/// \brief TEST 测试ModbusID
///
TEST(ModbusSlaveUnitTest, ModbusSlaveUnitTest_ID)
{
    DataInit();
    ModbusSlave::ModbusReplyStatus status;
    ModbusSlaveTest* test = new ModbusSlaveTest(ModbusSlaveTest::kModbusRtu);
    test->SetModbusID(1);

    //< 接收到的ID 为0时不回复

    recv_len = ModbusSlaveTest::SetBuffData(recv_buff,{0x00,0x03,0x00,0x01,0x00,0x01});
    recv_len = Crc::addCrc16(recv_buff,recv_len);
    status = test->slaveDataProcess(recv_buff,recv_len,send_buff,&send_len);
    EXPECT_EQ(status, ModbusSlave::kModbusSuccess);
    EXPECT_EQ(send_len, 0);

    //< 接收到的ID 为254时 正常回复
    recv_len = ModbusSlaveTest::SetBuffData(recv_buff,{0xfe,0x03,0x00,0x01,0x00,0x01});
    recv_len = Crc::addCrc16(recv_buff,recv_len);
    status = test->slaveDataProcess(recv_buff,recv_len,send_buff,&send_len);
    EXPECT_EQ(status, ModbusSlave::kModbusSuccess);
    EXPECT_EQ(send_buff[0], 1);

    delete test;
}

TEST(ModbusSlaveTcpUnitTest, ModbusSlaveUnitTest_ID)
{
    DataInit();
    ModbusSlave::ModbusReplyStatus status;
    ModbusSlaveTest* test = new ModbusSlaveTest(ModbusSlaveTest::kModbusTcp);
    test->SetModbusID(1);

    //< 接收到的ID 为0时不回复

    recv_len = ModbusSlaveTest::SetBuffData(recv_buff,{0x00,0x03,0x00,0x00,0x00,0x06,0x00,0x03,0x00,0x01,0x00,0x01});
    status = test->slaveDataProcess(recv_buff,recv_len,send_buff,&send_len);
    EXPECT_EQ(status, ModbusSlave::kModbusSuccess);
    EXPECT_EQ(send_len, 0);

    //< 接收到的ID 为254时 正常回复
    recv_len = ModbusSlaveTest::SetBuffData(recv_buff,{0x00,0x03,0x00,0x00,0x00,0x06,0xfe,0x03,0x00,0x01,0x00,0x01});
    status = test->slaveDataProcess(recv_buff,recv_len,send_buff,&send_len);
    EXPECT_EQ(status, ModbusSlave::kModbusSuccess);
    EXPECT_EQ(send_buff[6], 1);

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

TEST(ModbusSlaveTcpUnitTest, ModbusSlaveUnitTest_ReadHoldRegs)
{
    DataInit();
    ModbusSlaveTest* test = new ModbusSlaveTest(ModbusSlaveTest::kModbusTcp);

    recv_len = ModbusSlaveTest::SetBuffData(recv_buff,{0x00,0x03,0x00,0x00,0x00,0x06,0x01,0x03,0x00,0x01,0x00,0x01});
    test->SetModbusID(1);
    ModbusSlave::ModbusReplyStatus status = test->slaveDataProcess(recv_buff,recv_len,send_buff,&send_len);
    EXPECT_EQ(status, ModbusSlave::kModbusSuccess);
    EXPECT_EQ(send_buff[0],0x00);
    EXPECT_EQ(send_buff[1],0x03);
    EXPECT_EQ(send_buff[5],0x03);

    EXPECT_EQ(send_buff[6],0x01);
    EXPECT_EQ(send_buff[7],0x83);
    EXPECT_EQ(send_buff[8],0x02);

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

///
/// \brief TEST     测试写保持寄存器
///
/// 1. 设置从站id为1  地址1中的值为123
/// 2. 使用主站接口设置从站1 地址1 值为0xff
/// 3. 判断从站1 地址1 中的值是否为 0xff
///
TEST(ModbusMasterUnitTest, ModbusMasterUnitTest_WriteHoldRegs)
{
    uint16_t slave_id = 1;
    uint16_t read_addr = 0x01;
    int16_t value = 123;
    int16_t value1 = 0xff;

    DataInit();

    ModbusSlaveTest* slave = new ModbusSlaveTest(ModbusSlaveTest::kModbusRtu);
    slave->SetModbusID(slave_id);
    slave->hold_regs[read_addr] = value;


    ModbusMasterTest * master = new ModbusMasterTest(ModbusMasterTest::kModbusRtu);

    recv_len = ModbusSlaveTest::SetBuffData(recv_buff,
               {0x01,0x06,static_cast<unsigned char>((read_addr>>8)&0x0ff),static_cast<unsigned char>((read_addr&0xff)),static_cast<unsigned char>((value1>>8)&0x0ff),static_cast<unsigned char>((value1&0xff))});
    recv_len = Crc::addCrc16(recv_buff,recv_len);


    // 主站读 从站1 地址1 长度1
    master->masterWriteSingleRegister(slave_id,read_addr,value1);

    EXPECT_EQ(recv_len,8);
    for(uint32_t i = 0;i<recv_len;i++){
        EXPECT_EQ(master->send_buff[i],recv_buff[i]);
    }


    // 从站处理主站的数据
    ModbusSlave::ModbusReplyStatus status = slave->slaveDataProcess(recv_buff,recv_len,send_buff,&send_len);
    EXPECT_EQ(status, ModbusSlave::kModbusSuccess);

    // 主站处理从站的数据
    ModbusSlaveTest::ModbusErrorCode error = ModbusSlaveTest::kModbusExceptionNone;
    status = master->masterDataProcess(send_buff,send_len,&error);

    EXPECT_EQ(status, ModbusMasterTest::kModbusSuccess);
    EXPECT_EQ(error, ModbusMasterTest::kModbusExceptionNone);
    EXPECT_EQ(value1,slave->hold_regs[read_addr]);

    delete slave;
    delete master;
}


TEST(ModbusMasterUnitTest, ModbusMasterUnitTest_ReadCoils)
{
    uint16_t slave_id = 1;
    uint16_t read_addr = 0x01;
    int16_t value = 1;

    DataInit();

    ModbusSlaveTest* slave = new ModbusSlaveTest(ModbusSlaveTest::kModbusRtu);
    slave->SetModbusID(slave_id);

    ModbusMasterTest * master = new ModbusMasterTest(ModbusMasterTest::kModbusRtu);

    recv_len = ModbusSlaveTest::SetBuffData(recv_buff,
                                            {0x01,0x01,static_cast<unsigned char>((read_addr>>8)&0x0ff),static_cast<unsigned char>((read_addr&0xff)),0x00,0x01});


    recv_len = Crc::addCrc16(recv_buff,recv_len);
    // 主站读 从站1 地址1 长度1
    master->masterReadCoils(1,1,1);

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
