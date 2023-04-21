#ifndef TST_MODBUSTEST_H
#define TST_MODBUSTEST_H

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

using namespace testing;

TEST(ModbusTest, ModbusTest)
{
    EXPECT_EQ(1, 1);
    ASSERT_THAT(0, Eq(0));
}

#endif // TST_MODBUSTEST_H
