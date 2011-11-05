/* *************************************************
 * Filename:test_main.cpp  Framework for Logging and Design By Contract
 * Created: 2011 by Kjell Hedström
 *
 * PUBLIC DOMAIN and Not copywrited. First published at KjellKod.cc
 * ********************************************* */

#include <gtest/gtest.h>
#include <iostream>
#include <chrono>

int main(int argc, char *argv[])
{
  testing::InitGoogleTest(&argc, argv);
  int return_value = RUN_ALL_TESTS();
  std::cout << "FINISHED WITH THE TESTING" << std::endl;
  return return_value;
}

