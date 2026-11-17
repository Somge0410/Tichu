cmake_minimum_required(VERSION 3.16)
cmake_minimum_required(VERSION 3.16)
project(Tichu LANGUAGES CXX)

add_executable(Tichu
    src/main.cpp
)

target_include_directories(Tichu PRIVATE include)
target_compile_features(Tichu PRIVATE cxx_std_17)

if(MSVC)
    target_link_options(Tichu PRIVATE "$<$<CONFIG:RelWithDebInfo>:/PROFILE>")
endif()
