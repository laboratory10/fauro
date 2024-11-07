// ======================================================================
// \title  FswManagerTestMain.cpp
// \author laboratory10
// \brief  cpp file for FswManager component test main function
// ======================================================================

#include "FswManagerTester.hpp"

TEST(Nominal, toDo) {
  Components::FswManagerTester tester;
  tester.toDo();
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
