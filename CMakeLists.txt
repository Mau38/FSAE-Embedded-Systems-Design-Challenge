cmake_minimum_required(VERSION 3.15)
project(FSAEDesignChallengeProject LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(deployment
    src/BMS.c
    src/HAL.c
    src/HAL_sim.c
)

target_include_directories(deployment PRIVATE include)

