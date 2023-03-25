#include <gtest/gtest.h>
#include "Elevator.h"
TEST(testCase, test1)
{
	Elevator elevator;
	
	EXPECT_EQ(elevator.get_floor(), 1);
}
TEST(testCase, test2)
{
	Elevator elevator;
	elevator.set_id(1);
	
	elevator.move(5);
	EXPECT_EQ(elevator.get_floor(), 5);
}
TEST(testCase, test3)
{
	Elevator elevator;
	elevator.set_id(1);
	
	elevator.callElevator(10, 1);
	EXPECT_EQ(elevator.get_floor(), 1);
}
int main(int argc, char **argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
