#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>
#include <windows>
#include <memory> 
#include <iostream> 

using namespace g3;


 int WINAPI WinMain(HINSTANCE hInstance,
	 HINSTANCE hPrevInstance,
	 LPSTR lpCmdLine,
	 int nCmdShow) {

	 std::unique_ptr<LogWorker> logworker{ LogWorker::createLogWorker() };

	 try
	 {
		 auto sinkHandle = logworker->addDefaultLogger("example", "./logs");
		 initializeLogging(logworker.get());
			 g3::log_levels::setHighest(G3LOG_DEBUG);
	 }
	 catch (std::exception e)
	 {
		 std::cerr << "Logging exception found: " << e.what() << "\n";
		 std::cerr << "Logging disabled for this session.\n";
	 }
        return 0;
}
