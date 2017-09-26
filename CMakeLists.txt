cmake_minimum_required(VERSION 2.8)
project(mc-pricer CXX)

# Prefix
if (NOT PREFIX)
    set(PREFIX ${CMAKE_CURRENT_BINARY_DIR})
endif (NOT PREFIX)
set(CMAKE_INSTALL_PREFIX ${PREFIX} CACHE STRING "Installation prefix." FORCE)

if (CXX)
    set(CMAKE_CXX_COMPILER ${CXX})
endif (CXX)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

# Release or Debug
if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "Setting build type to 'Debug' as none was specified.")
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif ()

set(CMAKE_CTEST_COMMAND ctest -V) 
include(CTest)
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})

# Detect Boost
set(Boost_NO_BOOST_CMAKE ON)
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
message(STATUS "Boost include directory ${Boost_INCLUDE_DIR}")

# Detect PNL
find_package(Pnl REQUIRED)
set(LIBS ${LIBS} ${PNL_LIBRARIES})
include_directories(${PNL_INCLUDE_DIRS})
if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    add_definitions(-DPNL_RANGE_CHECK_OFF)
endif()

# Add math lib
set(LIBS ${LIBS} m)

# Testing the parser
add_executable(testParser src/parser.cpp test/TestParser.cpp)
target_link_libraries(testParser ${LIBS})

# Test Option
add_executable(testOption test/TestOption.cpp src/BlackScholesModel.cpp src/MonteCarlo.cpp src/OptionAsian.cpp src/OptionBasket.cpp src/Option.cpp src/OptionPerformance.cpp src/OptionPerformance.hpp)
target_link_libraries(testOption ${LIBS})
add_test(testOption testOption)

# Test BlackScholesModel
add_executable(testBlackScholes src/MonteCarlo.cpp src/Option.cpp src/OptionPerformance.cpp src/OptionBasket.cpp src/OptionAsian.cpp src/BlackScholesModel.cpp src/parser.cpp test/TestBlackScholes.cpp)
target_link_libraries(testBlackScholes ${LIBS})
add_test(testBlackScholes testBlackScholes)

# Test MC
add_executable(testMC src/parser.cpp test/TestMC.cpp src/BlackScholesModel.cpp src/MonteCarlo.cpp src/OptionAsian.cpp src/OptionBasket.cpp src/Option.cpp src/OptionPerformance.cpp)
target_link_libraries(testMC ${LIBS})
#add_test(testMC_basket testMC ../data/basket.dat)
#add_test(testMC_basket_1 testMC ../data/basket_1.dat)
#add_test(testMC_basket_2 testMC ../data/basket_2.dat)
#add_test(testMC_asian testMC ../data/asian.dat)
#add_test(testMC_perf testMC ../data/perf.dat)

# Test PricerHistory (t > 0)
add_executable(testPricerHistory src/parser.cpp test/TestPricerHistory.cpp src/BlackScholesModel.cpp src/MonteCarlo.cpp src/OptionAsian.cpp src/OptionBasket.cpp src/Option.cpp src/OptionPerformance.cpp src/HedgingPortfolio.cpp)
target_link_libraries(testPricerHistory ${LIBS})
add_test(testPricerHistory_basket testPricerHistory ../data/market-data/simul_basket.dat)
add_test(testPricerHistory_basket_1 testPricerHistory ../data/market-data/simul_basket_1.dat)
add_test(testPricerHistory_basket_2 testPricerHistory ../data/market-data/simul_basket_2.dat)
add_test(testPricerHistory_asian testPricerHistory ../data/market-data/simul_asian.dat)
add_test(testPricerHistory_perf testPricerHistory ../data/market-data/simul_perf.dat)

# Test BlackScholesModel::simul_market
add_executable(testBlackScholesModel_simul_market test/TestSimulMarket.cpp src/BlackScholesModel.cpp src/parser.cpp)
target_link_libraries(testBlackScholesModel_simul_market ${LIBS})
add_test(testBlackScholesModel_simul_market testBlackScholesModel_simul_market)

# add_executable(mc-pricer src/parser.cpp src/pricer.cpp)
# target_link_libraries(mc-pricer ${LIBS})
# install(TARGETS mc-pricer DESTINATION bin)


# Print compiler flags
get_directory_property(DirDefs COMPILE_DEFINITIONS)
message(STATUS "COMPILE_DEFINITIONS = ${DirDefs}")
if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
    message(STATUS "C++ Flags: ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE} ${CMAKE_LINKER_EXE_FLAGS}" )
else()
    message(STATUS "C++ Flags: ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} ${CMAKE_LINKER_EXE_FLAGS}" )
endif()
