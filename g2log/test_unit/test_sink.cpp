#include <gtest/gtest.h>
#include "g2log.h"
#include <memory>
#include <string>

#include "testing_helpers.h"
#include "std2_make_unique.hpp"

using namespace std;
using namespace std2;

//class CoutSink{
//  stringstream buffer;
//  unique_ptr<ScopedCout> scope_ptr;
//  CoutSink() : scope_ptr(make_unique<ScopedCout>(&buffer){}
//};
// 
