#include <gtest/gtest.h>
#include "g2log.h"
#include <memory>
#include <string>

#include "testing_helpers.h"
#include "std2_make_unique.hpp"

using namespace std;
using namespace std2;

class CoutSink {
  stringstream buffer;
  unique_ptr<ScopedCout> scope_ptr;

  CoutSink() : scope_ptr(std2::make_unique<ScopedCout>(&buffer)) {}
  void save(g2::internal::LogEntry msg) { std::cout << msg;  }
  void clear() { buffer.str("");  }
  std::string string() { return buffer.str();  }

public:
  virtual ~CoutSink() final {}

  static std::unique_ptr<CoutSink> createSink() {
    return std::unique_ptr<CoutSink>(new CoutSink);
  }
};


TEST(SinkBasics, UniquePtr_CoutSink) {
  auto ptr = CoutSink::createSink();
  ASSERT_FALSE(nullptr == ptr);
  ptr.release();
  ASSERT_TRUE(nullptr == ptr);
}

TEST(SinkToLogger, AddSink_Expect_return_Handle) {
    //auto ptr = CoutSink::createSink();
    //RestoreLogger raii_logger;
    //auto handler = raii_logger.callToLogger(&addSink, CoutSink::createSink());
    //ASSERT_FALSE(nullptr == handler);
  ASSERT_FALSE(true);
}