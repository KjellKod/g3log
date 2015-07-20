/** ==========================================================================
* 2014 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
 *
 * For more information see g3log/LICENSE or refer refer to http://unlicense.org
* ============================================================================*/

#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>

#include <iostream>
#include <cctype>
#include <future>
#include <vector>
#include <string>
#include <chrono>
#include <thread>


namespace
{
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
   const std::string path_to_log_file = "./";
#else
   const std::string path_to_log_file = "/tmp/";
#endif

   void ToLower(std::string &str)
   {
      for (auto &character : str) {
         character = std::tolower(character);
      }
   }

   void RaiseSIGABRT() {
      std::cout << "Calling :" << __FUNCTION__ << " Line: " << __LINE__ << std::endl << std::flush;
      raise(SIGABRT);
      LOG(WARNING) << "Expected to have died by now...";
   }

   void RaiseSIGFPE() {
      std::cout << "Calling :" << __FUNCTION__ << " Line: " << __LINE__ << std::endl << std::flush;
      LOGF_IF(INFO, (false != true), "Exiting %s SIGFPE", "by");
      raise(SIGFPE);
      LOG(WARNING) << "Expected to have died by now...";
   }

   void RaiseSIGSEGV() {
      std::cout << "Calling :" << __FUNCTION__ << " Line: " << __LINE__ << std::endl << std::flush;
      LOG(DEBUG) << "Exit by SIGSEGV";
      raise(SIGSEGV);
      LOG(WARNING) << "Expected to have died by now...";
   }

   void RaiseSIGILL() {
      std::cout << "Calling :" << __FUNCTION__ << " Line: " << __LINE__ << std::endl << std::flush;
      LOGF(DEBUG, "Exit by %s", "SIGILL");
      raise(SIGILL);
      LOG(WARNING) << "Expected to have died by now...";
   }

   void RAiseSIGTERM() {
      std::cout << "Calling :" << __FUNCTION__ << " Line: " << __LINE__ << std::endl << std::flush;
      LOGF_IF(INFO, (false != true), "Exiting %s SIGFPE", "by");
      raise(SIGTERM);
      LOG(WARNING) << "Expected to have died by now...";
   }

   int gShouldBeZero = 1;
   void DivisionByZero() {
      std::cout << "Calling :" << __FUNCTION__ << " Line: " << __LINE__ << std::endl << std::flush;
      std::cout << "Executing DivisionByZero: gShouldBeZero: "  << gShouldBeZero << std::endl;
      LOG(INFO) << "Division by zero is a big no-no";
      int value = 3;
      auto test = value / gShouldBeZero;
      LOG(WARNING) << "Expected to have died by now..., test value: " << test;
   }

   void IllegalPrintf() {
      std::cout << "Calling :" << __FUNCTION__ << " Line: " << __LINE__ << std::endl << std::flush;
      LOG(DEBUG) << "Impending doom due to illeteracy";
      LOGF(INFO, "2nd attempt at ILLEGAL PRINTF_SYNTAX %d EXAMPLE. %s %s", "hello", 1);
      LOG(WARNING) << "Expected to have died by now...";
   }

   void OutOfBoundsArrayIndexing() {
      std::cout << "Calling :" << __FUNCTION__ << " Line: " << __LINE__ << std::endl << std::flush;
      std::vector<int> v;
      v[0] = 5;
      LOG(WARNING) << "Expected to have died by now...";
   }


   void AccessViolation() {
      std::cout << "Calling :" << __FUNCTION__ << " Line: " << __LINE__ << std::endl << std::flush;
      char *ptr = 0;
      LOG(INFO) << "Death by access violation is imminent";
      *ptr = 0;
      LOG(WARNING) << "Expected to have died by now...";
   }

   void NoExitFunction() {
      std::cout << "Calling :" << __FUNCTION__ << " Line: " << __LINE__ << std::endl << std::flush;
      CHECK(false) << "This function should never be called";
   }

   void RaiseSIGABRTAndAccessViolation() {
      std::cout << "Calling :" << __FUNCTION__ << " Line: " << __LINE__ << std::endl << std::flush;

      auto f1 = std::async(std::launch::async, &RaiseSIGABRT);
      auto f2 = std::async(std::launch::async, &AccessViolation);
      f1.wait();
      f2.wait();
   }

   void ThrowInt() {
      throw 1233210;
   }

   void FailedCHECK() {
      CHECK(false) << "This is fatal";
   }

   void CallActualExitFunction(std::function<void()> fatal_function) {
      fatal_function();
   }

   void CallExitFunction(std::function<void()> fatal_function) {
      CallActualExitFunction(fatal_function);
   }



   void ExecuteDeathFunction(const bool runInNewThread, int fatalChoice) {
      std::cout << "Calling :" << __FUNCTION__ << " Line: " << __LINE__ << std::endl << std::flush;
      auto exitFunction = &NoExitFunction;
      switch (fatalChoice) {
      case 1: exitFunction = &RaiseSIGABRT;  break;
      case 2: exitFunction = &RaiseSIGFPE;  break;
      case 3: exitFunction = &RaiseSIGSEGV;  break;
      case 4: exitFunction = &RaiseSIGILL;  break;
      case 5: exitFunction = &RAiseSIGTERM;  break;
      case 6: exitFunction = &DivisionByZero;  gShouldBeZero = 0; DivisionByZero();  break;
      case 7: exitFunction = &IllegalPrintf;  break;
      case 8: exitFunction = &OutOfBoundsArrayIndexing;  break;
      case 9: exitFunction = &AccessViolation;  break;
      case 10: exitFunction = &RaiseSIGABRTAndAccessViolation; break;
      case 11: exitFunction = &ThrowInt; break;
      case 12: exitFunction = &FailedCHECK; break;
      default: break;
      }
      if (runInNewThread) {
         auto dieInNearFuture = std::async(std::launch::async, CallExitFunction, exitFunction);
         dieInNearFuture.wait();
      } else {
         CallExitFunction(exitFunction);
      }

      std::string unexpected = "Expected to exit by FATAL event. That did not happen (printf choice in Windows?).";
      unexpected.append("Choice was: ").append(std::to_string(fatalChoice)).append(", async?: ")
      .append(std::to_string(runInNewThread)).append("\n\n***** TEST WILL RUN AGAIN *****\n\n");

      std::cerr << unexpected  << std::endl;
      LOG(WARNING) << unexpected;

   }

   bool AskForAsyncDeath() {
      std::string option;
      while (true) {
         option.clear();
         std::cout << "Do you want to run the test in a separate thread? [yes/no]" << std::endl;
         std::getline(std::cin, option);
         ToLower(option);
         if (("yes" != option) && ("no" != option)) {
            std::cout << "\nInvalid value: [" << option << "]\n\n\n";
         } else {
            break;
         }
      }
      return ("yes" == option);
   }



   int ChoiceOfFatalExit() {
      std::string option;
      int choice = {0};

      while (true) {
         std::cout << "\n\n\n\nChoose your exit" << std::endl;
         std::cout << "By throwing an fatal signal" << std::endl;
         std::cout << "or By executing a fatal code snippet" << std::endl;
         std::cout << "[1] Signal SIGABRT" << std::endl;
         std::cout << "[2] Signal SIGFPE" << std::endl;
         std::cout << "[3] Signal SIGSEGV" << std::endl;
         std::cout << "[4] Signal IGILL" << std::endl;
         std::cout << "[5] Signal SIGTERM" << std::endl;

         std::cout << "[6] Division By Zero" << std::endl;
         std::cout << "[7] Illegal printf" << std::endl;
         std::cout << "[8] Out of bounds array indexing  " << std::endl;
         std::cout << "[9] Access violation" << std::endl;
         std::cout << "[10] Rasing SIGABRT + Access Violation in two separate threads" << std::endl;
         std::cout << "[11] Just throw (in this thread)" << std::endl;
         std::cout << "[12] Just CHECK(false) (in this thread)" << std::endl;


         std::cout << std::flush;

         try {
            std::getline(std::cin, option);
            choice = std::stoi(option);
            if (choice <= 0 || choice > 12) {
               std::cout << "Invalid choice: [" << option << "\n\n";
            }  else {
               return choice;
            }
         } catch (...) {
            std::cout << "Invalid choice: [" << option << "\n\n";
         }
      }
   }

   void ForwardChoiceForFatalExit(bool runInNewThread, int fatalChoice) {
      ExecuteDeathFunction(runInNewThread, fatalChoice);
   }

   void ChooseFatalExit() {
      const bool runInNewThread = AskForAsyncDeath();
      const int exitChoice = ChoiceOfFatalExit();
      ForwardChoiceForFatalExit(runInNewThread, exitChoice);
   }
} // namespace

void breakHere() {
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
   __debugbreak();
#endif
}

int main(int argc, char **argv)
{
   auto logger_n_handle = g3::LogWorker::createWithDefaultLogger(argv[0], path_to_log_file);
   g3::initializeLogging(logger_n_handle.worker.get());
   g3::setFatalPreLoggingHook(&breakHere);

   std::future<std::string> log_file_name = logger_n_handle.sink->call(&g3::FileSink::fileName);
   std::cout << "**** G3LOG FATAL EXAMPLE ***\n\n"
             << "Choose your type of fatal exit, then "
             << " read the generated log and backtrace.\n"
             << "The logfile is generated at:  [" << log_file_name.get() << "]\n\n" << std::endl;


   LOGF(DEBUG, "Fatal exit example starts now, it's as easy as  %d", 123);
   LOG(INFO) << "Feel free to read the source code also in g3log/example/main_fatal_choice.cpp";

   while (true) {
      ChooseFatalExit();
   }

   LOG(WARNING) << "Expected to exit by fatal event, this code line should never be reached";
   CHECK(false) << "Forced death";
   return 0;

}

