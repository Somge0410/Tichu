cmake_minimum_required(VERSION 3.16)
cmake_minimum_required(VERSION 3.16)
project(Tichu LANGUAGES CXX)

add_library(tichu_core
    src/types.cpp
    src/cards.cpp
    src/move_generation.cpp
    src/game_state.cpp
    src/player.cpp
    src/game.cpp
)

target_include_directories(tichu_core PUBLIC include)
target_compile_features(tichu_core PUBLIC cxx_std_17)

add_executable(Tichu src/main.cpp)
target_link_libraries(Tichu PRIVATE tichu_core)
target_compile_definitions(Tichu PRIVATE "$<$<CONFIG:RelWithDebInfo>:TICHU_PROFILE_BUILD>")

if(MSVC)
    target_link_options(Tichu PRIVATE "$<$<CONFIG:RelWithDebInfo>:/PROFILE>")
endif()

include(CTest)
if(BUILD_TESTING)
    add_executable(TichuTests tests/rules_tests.cpp)
    target_link_libraries(TichuTests PRIVATE tichu_core)
    add_test(NAME TichuRules COMMAND TichuTests)
endif()

include(CheckIPOSupported)
check_ipo_supported(RESULT tichu_ipo_supported OUTPUT tichu_ipo_error LANGUAGES CXX)
if(tichu_ipo_supported)
    foreach(tichu_target tichu_core Tichu TichuTests)
        if(TARGET ${tichu_target})
            set_target_properties(${tichu_target} PROPERTIES
                INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE
                INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO TRUE
            )
        endif()
    endforeach()
    message(STATUS "IPO/LTO enabled for Release and RelWithDebInfo")
else()
    message(WARNING "IPO/LTO is unavailable: ${tichu_ipo_error}")
endif()
