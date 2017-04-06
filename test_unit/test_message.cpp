/** ==========================================================================
* 2016 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
 *
 * For more information see g3log/LICENSE or refer refer to http://unlicense.org
* ============================================================================*/

#include <gtest/gtest.h>
#include <g3log/g3log.hpp>
#include <g3log/time.hpp>
#include <iostream>
#include <ctime>

TEST(Message, CppSupport) {
   // ref: http://www.cplusplus.com/reference/clibrary/ctime/strftime/
   // ref: http://en.cppreference.com/w/cpp/io/manip/put_time
   //  Day Month Date Time Year: is written as "%a %b %d %H:%M:%S %Y" and formatted output as : Wed Sep 19 08:28:16 2012
   // --- WARNING: The try/catch setup does NOT work,. but for fun and for fake-clarity I leave it
   // ---  For formatting options to std::put_time that are NOT YET implemented on Windows fatal errors/assert will occurr
   // ---  the last example is such an example.
   try {
      std::cout << g3::localtime_formatted(g3::systemtime_now(), "%a %b %d %H:%M:%S %Y")  << std::endl;
      std::this_thread::sleep_for(std::chrono::seconds(1));
      std::cout << g3::localtime_formatted(g3::systemtime_now(), "%%Y/%%m/%%d %%H:%%M:%%S = %Y/%m/%d %H:%M:%S")  << std::endl;
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
      std::cerr << "Formatting options skipped due to VS2012, C++11 non-conformance for" << std::endl;
      std::cerr << " some formatting options. The skipped code was:\n\t\t %EX %Ec, \n(see http://en.cppreference.com/w/cpp/io/manip/put_time for details)"  << std::endl;
#else
      std::cout << "C++11 new formatting options:\n" << g3::localtime_formatted(g3::systemtime_now(), "%%EX: %EX\n%%z: %z\n%%Ec: %Ec")  << std::endl;
#endif
   }
// This does not work. Other kinds of fatal exits (on Windows) seems to be used instead of exceptions
// Maybe a signal handler catch would be better? --- TODO: Make it better, both failing and correct
   catch (...) {
      ADD_FAILURE() << "On this platform the library does not support given (C++11?) specifiers";
      return;
   }
   ASSERT_TRUE(true); // no exception. all good
}



TEST(Message, GetFractional_Empty_buffer_ExpectDefaults) {
   auto fractional = g3::internal::getFractional("", 0);
   const auto expected = g3::internal::Fractional::NanosecondDefault;
   EXPECT_EQ(fractional, expected);
   fractional = g3::internal::getFractional("", 100);
   EXPECT_EQ(fractional, expected);
}

TEST(Message, GetFractional_MilliSeconds) {
   auto fractional = g3::internal::getFractional("%f3", 0);
   const auto expected = g3::internal::Fractional::Millisecond;
   EXPECT_EQ(fractional, expected);
}

TEST(Message, GetFractional_Microsecond) {
   auto fractional = g3::internal::getFractional("%f6", 0);
   const auto expected = g3::internal::Fractional::Microsecond;
   EXPECT_EQ(fractional, expected);
}

TEST(Message, GetFractional_Nanosecond) {
   auto fractional = g3::internal::getFractional("%f9", 0);
   const auto expected = g3::internal::Fractional::Nanosecond;
   EXPECT_EQ(fractional, expected);
}

TEST(Message, GetFractional_NanosecondDefault) {
   auto fractional = g3::internal::getFractional("%f", 0);
   const auto expected = g3::internal::Fractional::NanosecondDefault;
   EXPECT_EQ(fractional, expected);
}

TEST(Message, GetFractional_All) {
   std::string formatted = "%f, %f9, %f6, %f3";
   auto fractional = g3::internal::getFractional(formatted, 0);
   auto expected = g3::internal::Fractional::NanosecondDefault;
   EXPECT_EQ(fractional, expected);

   // ns
   fractional = g3::internal::getFractional(formatted, 4);
   expected = g3::internal::Fractional::Nanosecond;
   EXPECT_EQ(fractional, expected);

   // us
   fractional = g3::internal::getFractional(formatted, 9);
   expected = g3::internal::Fractional::Microsecond;
   EXPECT_EQ(fractional, expected);

   // ms
   fractional = g3::internal::getFractional(formatted, 14);
   expected = g3::internal::Fractional::Millisecond;
   EXPECT_EQ(fractional, expected);
}

TEST(Message, FractionalToString) {
   timespec ts = {};
   ts.tv_nsec = 123456789;
   auto value = g3::internal::to_string(ts, g3::internal::Fractional::Nanosecond);
   EXPECT_EQ("123456789", value);
   value = g3::internal::to_string(ts, g3::internal::Fractional::NanosecondDefault);
   EXPECT_EQ("123456789", value);

   // us
   value = g3::internal::to_string(ts, g3::internal::Fractional::Microsecond);
   EXPECT_EQ("123456", value);
// ms
   value = g3::internal::to_string(ts, g3::internal::Fractional::Millisecond);
   EXPECT_EQ("123", value);
}

TEST(Message, FractionalToStringNanoPadded) {
   timespec ts = {};
   ts.tv_nsec = 1;
   auto value = g3::internal::to_string(ts, g3::internal::Fractional::Nanosecond);
   EXPECT_EQ("000000001", value);
   // 0000000012
   value = g3::internal::to_string(ts, g3::internal::Fractional::NanosecondDefault);
   EXPECT_EQ("000000001", value);
}

TEST(Message, FractionalToString12NanoPadded) {
   timespec ts = {};
   ts.tv_nsec = 12;
   auto value = g3::internal::to_string(ts, g3::internal::Fractional::Nanosecond);
   EXPECT_EQ("000000012", value);
   // 0000000012
   value = g3::internal::to_string(ts, g3::internal::Fractional::NanosecondDefault);
   EXPECT_EQ("000000012", value);
}


TEST(Message, FractionalToStringMicroPadded) {
   timespec ts = {};
   ts.tv_nsec = 1000;
   auto value = g3::internal::to_string(ts, g3::internal::Fractional::Microsecond);
   EXPECT_EQ("000001", value);
   ts.tv_nsec = 11000;
   value = g3::internal::to_string(ts, g3::internal::Fractional::Microsecond);
   EXPECT_EQ("000011", value);

}


TEST(Message, FractionalToStringMilliPadded) {
   timespec ts = {};
   ts.tv_nsec = 1000000;
   auto value = g3::internal::to_string(ts, g3::internal::Fractional::Millisecond);
   EXPECT_EQ("001", value);
   ts.tv_nsec = 21000000;
   value = g3::internal::to_string(ts, g3::internal::Fractional::Millisecond);
   EXPECT_EQ("021", value);
}



#if !(defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
TEST(Message, localtime_formatted) {
   struct tm tm;
   time_t t;

   ASSERT_TRUE(nullptr != strptime("2016-08-09 22:58:45", "%Y-%m-%d %H:%M:%S", &tm));
   t = mktime(&tm);
   timespec ts = {};
   ts.tv_sec = t;
   ts.tv_nsec = 123;
   auto format = g3::localtime_formatted(ts, g3::internal::date_formatted); // %Y/%m/%d
   EXPECT_EQ("2016/08/09", format);

   auto us_format = g3::localtime_formatted(ts, g3::internal::time_formatted); // "%H:%M:%S %f6";
   EXPECT_EQ("22:58:45 000000", us_format);

   auto ns_format = g3::localtime_formatted(ts, "%H:%M:%S %f");
   EXPECT_EQ("22:58:45 000000123", ns_format);

   ts.tv_nsec = 1234000;
   auto ms_format = g3::localtime_formatted(ts, "%H:%M:%S %f3");
   EXPECT_EQ("22:58:45 001", ms_format);
}
#endif


#ifdef G3_DYNAMIC_LOGGING
namespace {
   using LevelsContainer = std::map<int, g3::LoggingLevel>;
   const LevelsContainer g_test_log_level_defaults = {
      {g3::kDebugValue, {DEBUG}},
      {INFO.value, {INFO}},
      {WARNING.value, {WARNING}},
      {FATAL.value, {FATAL}}
   };

   const LevelsContainer g_test_all_disabled = {
      {g3::kDebugValue, {DEBUG, false}},
      {INFO.value, {INFO, false}},
      {WARNING.value, {WARNING, false}},
      {FATAL.value, {FATAL, false}}
   };


   bool mapCompare (LevelsContainer const& lhs, LevelsContainer const& rhs) {
      auto pred = [] (auto a, auto b) {
         return (a.first == b.first) &&
                (a.second == b.second);
      };

      return lhs.size() == rhs.size()
             && std::equal(lhs.begin(), lhs.end(), rhs.begin(), pred);
   }



} // anonymous
TEST(Level, Default) {
   g3::only_change_at_initialization::reset();
   auto defaults = g3::log_levels::getAll();
   EXPECT_EQ(defaults.size(), g_test_log_level_defaults.size());
   EXPECT_TRUE(mapCompare(defaults, g_test_log_level_defaults));
}

TEST(Level, DefaultChanged_only_change_at_initialization) {
   g3::only_change_at_initialization::reset();
   std::shared_ptr<void> RaiiLeveReset(nullptr, [&](void*) {
      g3::only_change_at_initialization::reset();
   });

   g3::only_change_at_initialization::addLogLevel(INFO, false);
   auto defaults = g3::log_levels::getAll();
   EXPECT_EQ(defaults.size(), g_test_log_level_defaults.size());
   EXPECT_FALSE(mapCompare(defaults, g_test_log_level_defaults));

   const LevelsContainer defaultsWithInfoChangged = {
      {g3::kDebugValue, {DEBUG, true}},
      {INFO.value, {INFO, false}},
      {WARNING.value, {WARNING, true}},
      {FATAL.value, {FATAL, true}}
   };
   EXPECT_TRUE(mapCompare(defaults, defaultsWithInfoChangged));
}

TEST(Level, DefaultChanged_log_levels) {
   g3::only_change_at_initialization::reset();
   std::shared_ptr<void> RaiiLeveReset(nullptr, [&](void*) {
      g3::only_change_at_initialization::reset();
   });

   g3::log_levels::disable(INFO);
   auto defaults = g3::log_levels::getAll();
   EXPECT_EQ(defaults.size(), g_test_log_level_defaults.size());
   EXPECT_FALSE(mapCompare(defaults, g_test_log_level_defaults));

   const LevelsContainer defaultsWithInfoChangged = {
      {g3::kDebugValue, {DEBUG, true}},
      {INFO.value, {INFO, false}},
      {WARNING.value, {WARNING, true}},
      {FATAL.value, {FATAL, true}}
   };
   EXPECT_TRUE(mapCompare(defaults, defaultsWithInfoChangged));
}

TEST(Level, Reset) {
   std::shared_ptr<void> RaiiLeveReset(nullptr, [&](void*) {
      g3::only_change_at_initialization::reset();
   });

   g3::log_levels::disableAll();
   auto all_levels = g3::log_levels::getAll();
   EXPECT_TRUE(mapCompare(all_levels, g_test_all_disabled));

   g3::only_change_at_initialization::reset();
   all_levels = g3::log_levels::getAll();
   EXPECT_TRUE(mapCompare(all_levels, g_test_log_level_defaults));



}



TEST(Level, AllDisabled) {
   g3::only_change_at_initialization::reset();
   std::shared_ptr<void> RaiiLeveReset(nullptr, [&](void*) {
      g3::only_change_at_initialization::reset();
   });


   auto all_levels = g3::log_levels::getAll();
   EXPECT_EQ(all_levels.size(), g_test_all_disabled.size());
   EXPECT_FALSE(mapCompare(all_levels, g_test_all_disabled));

   g3::log_levels::disableAll();
   all_levels = g3::log_levels::getAll();
   EXPECT_TRUE(mapCompare(all_levels, g_test_all_disabled));
}


TEST(Level, setHighestLogLevel_high_end) {
   g3::only_change_at_initialization::reset();
   std::shared_ptr<void> RaiiLeveReset(nullptr, [&](void*) {
      g3::only_change_at_initialization::reset();
   });


      g3::log_levels::enableAll();
      g3::log_levels::disable(FATAL);
      g3::log_levels::setHighest(FATAL);      


      LevelsContainer expected = {
      {g3::kDebugValue, {DEBUG, false}},
      {INFO.value, {INFO, false}},
      {WARNING.value, {WARNING, false}},
      {FATAL.value, {FATAL, true}}
   };

   auto all_levels = g3::log_levels::getAll();
   EXPECT_TRUE(mapCompare(all_levels, expected)) << g3::log_levels::to_string();
}


TEST(Level, setHighestLogLevel_low_end) {
   g3::only_change_at_initialization::reset();
   std::shared_ptr<void> RaiiLeveReset(nullptr, [&](void*) {
      g3::only_change_at_initialization::reset();
   });


      g3::log_levels::disableAll();
      g3::log_levels::setHighest(DEBUG);      


      LevelsContainer expected = {
      {g3::kDebugValue, {DEBUG, true}},
      {INFO.value, {INFO, true}},
      {WARNING.value, {WARNING, true}},
      {FATAL.value, {FATAL, true}}
   };

   auto all_levels = g3::log_levels::getAll();
   EXPECT_TRUE(mapCompare(all_levels, expected)) << g3::log_levels::to_string();
}


TEST(Level, setHighestLogLevel_middle) {
   g3::only_change_at_initialization::reset();
   std::shared_ptr<void> RaiiLeveReset(nullptr, [&](void*) {
      g3::only_change_at_initialization::reset();
   });


      g3::log_levels::enableAll();
      g3::log_levels::setHighest(WARNING);      


      LevelsContainer expected = {
      {g3::kDebugValue, {DEBUG, false}},
      {INFO.value, {INFO, false}},
      {WARNING.value, {WARNING, true}},
      {FATAL.value, {FATAL, true}}
   };

   auto all_levels = g3::log_levels::getAll();
   EXPECT_TRUE(mapCompare(all_levels, expected));
}




TEST(Level, setHighestLogLevel_StepWiseDisableAll) {
   g3::only_change_at_initialization::reset();
   std::shared_ptr<void> RaiiLeveReset(nullptr, [&](void*) {
      g3::only_change_at_initialization::reset();
   });

   LevelsContainer changing_levels = {
      {g3::kDebugValue, {DEBUG, true}},
      {INFO.value, {INFO, true}},
      {WARNING.value, {WARNING, true}},
      {FATAL.value, {FATAL, true}}
   };

   auto all_levels = g3::log_levels::getAll();
   EXPECT_TRUE(mapCompare(all_levels, g_test_log_level_defaults));

   size_t counter = 0;
   for (auto& lvl : changing_levels) {
      g3::log_levels::setHighest(lvl.second.level);
      all_levels = g3::log_levels::getAll();

      ASSERT_TRUE(mapCompare(all_levels, changing_levels)) <<
            "counter: " << counter << "\nsystem:\n" <<
            g3::log_levels::to_string(all_levels) <<
            "\nexpected:\n" <<
            g3::log_levels::to_string(changing_levels);

      ++counter;
      if (counter != changing_levels.size()) {
         // for next round this level will be disabled
         lvl.second.status = false;
      }
   }


   // in the end all except the last should be disabled
   auto mostly_disabled = g_test_all_disabled;
   mostly_disabled[FATAL.value].status = true;
   EXPECT_TRUE(mapCompare(changing_levels, mostly_disabled));

   all_levels = g3::log_levels::getAll();
   EXPECT_TRUE(mapCompare(all_levels, mostly_disabled)) <<
         "\nsystem:\n" <<
         g3::log_levels::to_string(all_levels) <<
         "\nexpected:\n" <<
         g3::log_levels::to_string(mostly_disabled);
}

TEST(Level, Print) {
   g3::only_change_at_initialization::reset();
   std::string expected = std::string{"name: DEBUG level: 100 status: 1\n"}
                          + "name: INFO level: 300 status: 1\n"
                          + "name: WARNING level: 500 status: 1\n"
                          + "name: FATAL level: 1000 status: 1\n";
   EXPECT_EQ(g3::log_levels::to_string(), expected);
}

TEST(Level, AddOneEnabled_option1) {
   std::shared_ptr<void> RaiiLeveReset(nullptr, [&](void*) {
      g3::only_change_at_initialization::reset();
   });


   LEVELS MYINFO {WARNING.value + 1, "MyInfoLevel"};
   g3::only_change_at_initialization::addLogLevel(MYINFO, true);

   auto modified = g_test_log_level_defaults;
   modified[MYINFO.value] = MYINFO;

   auto all_levels = g3::log_levels::getAll();
   EXPECT_TRUE(mapCompare(modified, all_levels)) << "\nsystem:\n" <<
         g3::log_levels::to_string(all_levels) <<
         "\nexpected:\n" <<
         g3::log_levels::to_string(modified);

}

TEST(Level, AddOneEnabled_option2) {
   std::shared_ptr<void> RaiiLeveReset(nullptr, [&](void*) {
      g3::only_change_at_initialization::reset();
   });


   LEVELS MYINFO {WARNING.value + 1, "MyInfoLevel"};
   g3::only_change_at_initialization::addLogLevel(MYINFO);

   auto modified = g_test_log_level_defaults;
   modified[MYINFO.value] = MYINFO;

   auto all_levels = g3::log_levels::getAll();
   EXPECT_TRUE(mapCompare(modified, all_levels)) << "\nsystem:\n" <<
         g3::log_levels::to_string(all_levels) <<
         "\nexpected:\n" <<
         g3::log_levels::to_string(modified);

}




TEST(Level, Addlevel_using_addLevel) {
   std::shared_ptr<void> RaiiLeveReset(nullptr, [&](void*) {
      g3::only_change_at_initialization::reset();
   });

   LEVELS MYINFO {WARNING.value + 1, "MyInfoLevel"};
   auto status = g3::log_levels::getStatus(MYINFO);
   EXPECT_EQ(status, g3::log_levels::status::Absent);

   g3::only_change_at_initialization::addLogLevel(MYINFO);
   status = g3::log_levels::getStatus(MYINFO);
   EXPECT_EQ(status, g3::log_levels::status::Enabled);
}

TEST(Level, Addlevel_using_addLogLevel_disabled) {
   std::shared_ptr<void> RaiiLeveReset(nullptr, [&](void*) {
      g3::only_change_at_initialization::reset();
   });

   LEVELS MYINFO {WARNING.value + 1, "MyInfoLevel"};
   auto status = g3::log_levels::getStatus(MYINFO);
   EXPECT_EQ(status, g3::log_levels::status::Absent);

   g3::only_change_at_initialization::addLogLevel(MYINFO, false);
   status = g3::log_levels::getStatus(MYINFO);
   EXPECT_EQ(status, g3::log_levels::status::Disabled);
}

TEST(Level, Addlevel__disabled) {
   std::shared_ptr<void> RaiiLeveReset(nullptr, [&](void*) {
      g3::only_change_at_initialization::reset();
   });

   LEVELS MYINFO {WARNING.value + 1, "MyInfoLevel"};
   auto status = g3::log_levels::getStatus(MYINFO);
   EXPECT_EQ(status, g3::log_levels::status::Absent);

   g3::log_levels::enable(MYINFO);
   status = g3::log_levels::getStatus(MYINFO);
   EXPECT_EQ(status, g3::log_levels::status::Absent);

   g3::log_levels::set(MYINFO, true);
   status = g3::log_levels::getStatus(MYINFO);
   EXPECT_EQ(status, g3::log_levels::status::Absent);

   g3::only_change_at_initialization::addLogLevel(MYINFO, false);
   status = g3::log_levels::getStatus(MYINFO);
   EXPECT_EQ(status, g3::log_levels::status::Disabled);
}

TEST(Level, Addlevel__enabled) {
   std::shared_ptr<void> RaiiLeveReset(nullptr, [&](void*) {
      g3::only_change_at_initialization::reset();
   });

   LEVELS MYINFO {WARNING.value + 1, "MyInfoLevel"};
   auto status = g3::log_levels::getStatus(MYINFO);
   EXPECT_EQ(status, g3::log_levels::status::Absent);


   g3::only_change_at_initialization::addLogLevel(MYINFO);
   status = g3::log_levels::getStatus(MYINFO);
   EXPECT_EQ(status, g3::log_levels::status::Enabled);
}

#endif // G3_DYNAMIC_LOGGING

