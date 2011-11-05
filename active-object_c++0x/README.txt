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
# from the top directory (active-object_c++0x)
# mkdir build; cd build; cmake ..; make
#
# In case of CMake problems, Compile command line
#g++ src/main.cpp src/active.cpp -I /home/kjhm/Desktop/KjellKod/active-object_c++0x/src -o ActiveObj -std=c++0x -I/usr/include/justthread -pthread -ljustthread -lrt
#
# Or to simplify things even more, put all the source code in the same folder and run
# g++ main.cpp active.cpp -o ActiveObj -std=c++0x -I/usr/include/justthread -pthread -ljustthread -lrt
