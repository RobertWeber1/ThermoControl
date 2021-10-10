cmake_minimum_required(VERSION 3.0.0)

project(ThermoControl LANGUAGES CXX)

Include(FetchContent)

#-------------------------------------------------------------------------------

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v2.13.1)

FetchContent_MakeAvailable(Catch2)

#-------------------------------------------------------------------------------

FetchContent_Declare(
  web_ui
  GIT_REPOSITORY https://github.com/RobertWeber1/web_ui_helper.git
  GIT_TAG        main)

FetchContent_MakeAvailable(web_ui)

#-------------------------------------------------------------------------------

add_executable(test-ThermoControl
	test/catch.cc
	test/sensor.cc
	test/spi_bus.cc
)

target_include_directories(test-ThermoControl
	PRIVATE src/
)

target_link_libraries(test-ThermoControl
	PRIVATE Catch2::Catch2
	# PRIVATE web_ui
)

target_compile_options(test-ThermoControl
	PRIVATE -std=c++17
	PRIVATE -Wextra
	PRIVATE -Wall
	PRIVATE -Wpedantic
	)
