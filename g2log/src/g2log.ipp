/* 
 * File:   g2logg.ipp
 * Author: kjell
 *
 * Created on October 20, 2013, 8:47 PM
 */

#pragma once

#include "g2log.hpp"
#include "g2logmessage.hpp"
#include "g2logworker.hpp"
#include "g2logmessageimpl.hpp"
#include "g2loglevels.hpp"

#include <cassert>
#include <memory>
#include <mutex>
#include <thread>
#include <chrono>

namespace g2 {
   namespace internal {
      g2LogWorker* g_logger_instance = nullptr; // instantiated and OWNED somewhere else (main)
      std::string g_once_error;
      std::once_flag g_error_flag;
      std::once_flag g_retrieve_error_flag;

      /**
       * saveMesage to the LogWorker which will pass them to the sinks.
       *  
       * In the messup of calling LOG before instantiating the logger the first 
       * message will be saved,.. to be given later with an initialization warning
       */
      void saveMessage(g2::LogMessage log_entry) {
         using namespace internal;

         if (false == isLoggingInitialized()) {
            std::call_once(g_error_flag, [&]() {
               g_once_error = {"\n\nWARNING LOGGER NOT INSTANTIATED WHEN CALLING IT"
                  "\nAt least once tried to call the logger before instantiating it\n\n"};
               g_once_error.append("\nThe first message was: \n").append(log_entry.toString());
            });
            std::cerr << g_once_error << std::endl;
            return;
         }

         std::call_once(g_retrieve_error_flag, [&]() {
            if (false == g_once_error.empty()) {
               std::string empty = {""};
               g2::LogMessage error{std::make_shared<g2::LogMessageImpl>(empty, 0, empty, WARNING)};
               error.stream() << g_once_error;
               g_logger_instance->save(error);
            }
         });

         g_logger_instance->save(log_entry);
      }

      void fatalCallToLogger(FatalMessage message) {
         // real fatal call to loggr
         internal::g_logger_instance->fatal(message);
      }

      // By default this function pointer goes to \ref fatalCall;
      void (*g_fatal_to_g2logworker_function_ptr)(FatalMessage) = fatalCallToLogger;

      void fatalCallForUnitTest(FatalMessage fatal_message) {
         // mock fatal call, not to logger: used by unit test
         assert(internal::g_logger_instance != nullptr);
         internal::g_logger_instance->save(fatal_message.copyToLogMessage()); // calling 'save' instead of 'fatal'
         throw std::runtime_error(fatal_message.toString());
      }

      /** The default, initial, handling to send a 'fatal' event to g2logworker 
       * he caller will stay here, eternally, until the software is aborted
       *  During unit testing the sleep-loop will be interrupted by exception from 
       * @ref fatalCallForUnitTest */
      void fatalCall(FatalMessage message) {
         g_fatal_to_g2logworker_function_ptr(message);
         while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
         }
      }

      /** Used to  REPLACE fatalCallToLogger for fatalCallForUnitTest
       * This function switches the function pointer so that only
       *  'unitTest' mock-fatal calls are made. */
      void changeFatalInitHandlerForUnitTesting() {
         g_fatal_to_g2logworker_function_ptr = fatalCallForUnitTest;
      }
   } // internal
} // g2

