# ================WINDOWS==================
# On Windows
# mkdir build; cd build;
# cmake -G "Visual Studio 10" ..
# msbuild ActiveObj_by_KjellKod.sln
# Debug\ActiveObj_by_KjellKod.exe
#
#
# ================LINUX==================
# On Linux, make sure that the environmental variables are set either in .profile or otherwise
# PATH=$PATH:/usr/include/justthread
# export PATH=/usr/lib/:$PATH
#
# from a++0x__active_object do 
# mkdir build; cd build; cmake ..; make
#
# In case of CMake problems, Compile command line
#g++ src/main.cpp src/active.cpp -I /home/kjhm/Desktop/KjellKod/active-object_c++0x/src -o ActiveObj -std=c++0x -I/usr/include/justthread -pthread -ljustthread -lrt
#
# Or to simplify things even more, put all the source code in the same folder and run
# g++ main.cpp active.cpp -o ActiveObj -std=c++0x -I/usr/include/justthread -pthread -ljustthread -lrt
#
#

cmake_minimum_required (VERSION 2.6)
project (ActiveObjCpp0x) 

IF(UNIX)
    set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS_DEBUG} -pthread -I/usr/include/justthread") 

	# make the src directory available for test classes
	include_directories("/usr/include/justthread")
	include_directories(../src) 

	# create the test executable
        #add_executable(ActiveObjCpp0x ../src/main.cpp  ../src/active.cpp )
        add_executable(ActiveObjCpp0x ../src/main.cpp  ../src/active.cpp ../src/shared_queue.h ../src/active.h ../src/backgrounder.h)
    target_link_libraries(ActiveObjCpp0x justthread rt)

ENDIF(UNIX)


IF(WIN32)
	include_directories("C:/program files/JustSoftwareSolutions/JustThread/include")
	include_directories(../src) 
	add_executable(ActiveObjCpp0x ../src/main.cpp  ../src/active.cpp )

	#Visual Studio 2010 
	IF(CMAKE_CXX_COMPILER STREQUAL "C:/Program Files/Microsoft Visual Studio 10.0/VC/bin/cl.exe")
       target_link_libraries(ActiveObjCpp0x $ENV{PROGRAMFILES}/JustSoftwareSolutions/JustThread/lib/justthread_vc10_mdd.lib)
	ENDIF(CMAKE_CXX_COMPILER STREQUAL "C:/Program Files/Microsoft Visual Studio 10.0/VC/bin/cl.exe")

	# Visual Studio 2008 DOES NOT WORK
	# ===================================
	# 1. std::function must be std::tr1::function but 
	#    std::unique_ptr does not exist (ref C:\Program Files\Microsoft Visual Studio 9.0\VC\include\memory.h)
	# while it exist for Visual Studio 10 (ref C:\Program Files\Microsoft Visual Studio 10.0\VC\include\memory.h)
	# ...Also this bug was discovered...
	# Ref: https://connect.microsoft.com/VisualStudio/feedback/details/339810/tr1-functional-doesnt-work-when-compiled-with-fastcall
	#....
	# However if you decide to use it anyhow and replacing the unique_ptr for shared_ptr and using std::tr1 then the
	# following might be of use
	#IF(CMAKE_CXX_COMPILER STREQUAL "C:/Program Files/Microsoft Visual Studio 9.0/VC/bin/cl.exe")
	#	target_link_libraries(ActiveObjCpp0x $ENV{PROGRAMFILES}/JustSoftwareSolutions/JustThread/lib/justthread_vc90_mdd.lib)
	#ENDIF(CMAKE_CXX_COMPILER STREQUAL "C:/Program Files/Microsoft Visual Studio 9.0/VC/bin/cl.exe")
ENDIF(WIN32)




