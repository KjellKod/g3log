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
#include "crashhandler.hpp"

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

      /** Fatal call saved to logger. This will trigger SIGABRT or other fatal signal 
       * to exit the program. After saving the fatal message the calling thread
       * will sleep forever (i.e. until the background thread catches up, saves the fatal
       * message and kills the software with the fatal signal.
       */
      void fatalCallToLogger(FatalMessage message) {
         if (!internal::isLoggingInitialized()) {
            std::ostringstream error;
            error << "FATAL CALL but logger is NOT initialized\n"
                    << "SIGNAL: " << g2::internal::signalName(message.signal_id_)
                    << "\nMessage: \n" << message.toString() << std::flush;
            std::cerr << error;
            internal::exitWithDefaultSignalHandler(message.signal_id_);
         }

         g_logger_instance->fatal(message);

         while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
         }
      }


      // By default this function pointer goes to \ref fatalCallToLogger;
     std::function<void(FatalMessage) > g_fatal_to_g2logworker_function_ptr = fatalCallToLogger; 
     

     /** The default, initial, handling to send a 'fatal' event to g2logworker
       *  the caller will stay here, eternally, until the software is aborted
       * ... in the case of unit testing it is the given "Mock" fatalCall that will
       * define the behaviour.
       */
      void fatalCall(FatalMessage message) {
         g_fatal_to_g2logworker_function_ptr(message);

      }

      // REPLACE fatalCallToLogger for fatalCallForUnitTest
      // This function switches the function pointer so that only
      //      'unitTest' mock-fatal calls are made.
      void changeFatalInitHandlerForUnitTesting(std::function<void(FatalMessage) > fatal_call) {
         g_fatal_to_g2logworker_function_ptr = fatal_call;
      }
   } // internal
} // g2

