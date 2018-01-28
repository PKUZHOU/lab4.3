cmake_minimum_required(VERSION 3.5)
project(riscv_simulator)

set(CMAKE_CXX_STANDARD 11)
include_directories(include)
set(SOURCE_FILES
        include/Read_Elf.h
        include/Reg_def.h
        include/signal_def.h
        include/Simulation.h
        include/utilits.h
        Read_Elf.cpp
        Simulation.cpp)

add_executable(riscv_simulator ${SOURCE_FILES})
