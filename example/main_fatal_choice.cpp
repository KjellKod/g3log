/** ==========================================================================
* 2014 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
 *
 * For more information see g3log/LICENSE or refer refer to http://unlicense.org
* ============================================================================*/

#include <g2logworker.hpp>
#include <g2log.hpp>
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

void sleep_for(size_t seconds) {
  std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

void RaiseSIGABRT() {
   std::cout << "Calling :" << __FUNCTION__ << " Line: " << __LINE__ << std::endl << std::flush;
   sleep_for(2);
   raise(SIGABRT);
   LOG(WARNING) << "Expected to have died by now...";
}

void RaiseSIGFPE() {
   std::cout << "Calling :" << __FUNCTION__ << " Line: " << __LINE__ << std::endl << std::flush;
   sleep_for(2);
   raise(SIGFPE);
   LOG(WARNING) << "Expected to have died by now...";
}

void RaiseSIGSEGV() {
   std::cout << "Calling :" << __FUNCTION__ << " Line: " << __LINE__ << std::endl << std::flush;
   sleep_for(2);
   raise(SIGSEGV);
   LOG(WARNING) << "Expected to have died by now...";
}

void RaiseSIGILL() {
   std::cout << "Calling :" << __FUNCTION__ << " Line: " << __LINE__ << std::endl << std::flush;
   sleep_for(2);
   raise(SIGILL);
   LOG(WARNING) << "Expected to have died by now...";
}

void RAiseSIGTERM() {
   std::cout << "Calling :" << __FUNCTION__ << " Line: " << __LINE__ << std::endl << std::flush;
   sleep_for(2);
   raise(SIGTERM);
   LOG(WARNING) << "Expected to have died by now...";
}

int gShouldBeZero = 1;
void DivisionByZero() {
   std::cout << "Calling :" << __FUNCTION__ << " Line: " << __LINE__ << std::endl << std::flush;
   std::cout << "Executing DivisionByZero: gShouldBeZero: "  << gShouldBeZero << std::endl;
   sleep_for(2);
   int value = 3;
   auto test = value / gShouldBeZero;
   LOG(WARNING) << "Expected to have died by now...";
}

void IllegalPrintf() {
   std::cout << "Calling :" << __FUNCTION__ << " Line: " << __LINE__ << std::endl << std::flush;
   sleep_for(2);
   printf("ILLEGAL PRINTF_SYNTAX %d EXAMPLE. %s %s", "hello", 1);
   LOGF(INFO, "2nd attempt at ILLEGAL PRINTF_SYNTAX %d EXAMPLE. %s %s", "hello", 1);
   LOG(WARNING) << "Expected to have died by now...";
}

void OutOfBoundsArrayIndexing() {
   std::cout << "Calling :" << __FUNCTION__ << " Line: " << __LINE__ << std::endl << std::flush;
   sleep_for(2);
   std::vector<int> v;
   v[0] = 5;
   LOG(WARNING) << "Expected to have died by now...";
}


void AccessViolation() {
   std::cout << "Calling :" << __FUNCTION__ << " Line: " << __LINE__ << std::endl << std::flush;
   sleep_for(2);
   char *ptr = 0;
   *ptr = 0;
   LOG(WARNING) << "Expected to have died by now...";
}

void NoExitFunction() {
   std::cout << "Calling :" << __FUNCTION__ << " Line: " << __LINE__ << std::endl << std::flush;
   CHECK(false) << "This function should never be called";
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
   case 6: exitFunction = &DivisionByZero;  gShouldBeZero = 0; break;
   case 7: exitFunction = &IllegalPrintf;  break;
   case 8: exitFunction = &OutOfBoundsArrayIndexing;  break;
   case 9: exitFunction = &AccessViolation;  break;
   default: break;
   }
   if (runInNewThread) {
      auto dieInNearFuture = std::async(std::launch::async, exitFunction);
      dieInNearFuture.wait();
   } else {
      exitFunction();
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
      std::cout << "[9] Access violation  \n\n" << std::endl;
      std::cout << std::flush;

      try {
         std::getline(std::cin, option);
         choice = std::stoi(option);
         if (choice <= 0 || choice > 9) {
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

int main(int argc, char **argv)
{
   auto logger_n_handle = g2::LogWorker::createWithDefaultLogger(argv[0], path_to_log_file);
   g2::initializeLogging(logger_n_handle.worker.get());
   std::future<std::string> log_file_name = logger_n_handle.sink->call(&g2::FileSink::fileName);
   std::cout << "**** G3LOG FATAL EXAMPLE ***\n\n"
             << "Choose your type of fatal exit, then "
             << " read the generated log and backtrace.\n"
             << "The logfile is generated at:  [" << log_file_name.get() << "]\n\n" << std::endl;


   LOGF(INFO, "Fatal exit example starts now, it's as easy as  %d", 123);
   LOG(INFO) << "Feel free to read the source code also in g3log/example/main_fatal_choice.cpp";

   while (true) {
      ChooseFatalExit();
   }

   LOG(WARNING) << "Expected to exit by fatal event, this code line should never be reached";
   CHECK(false) << "Forced death";
   return 0;

}

