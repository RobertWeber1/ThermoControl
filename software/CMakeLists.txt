cmake_minimum_required(VERSION 3.0.0)

project(ThermoControl LANGUAGES CXX C ASM)

Include(FetchContent)

#-------------------------------------------------------------------------------

# FetchContent_Declare(
#   arduino_cmake
#   GIT_REPOSITORY https://github.com/makarenya/esp8266-arduino-cmake.git
#   GIT_TAG        master)

# FetchContent_MakeAvailable(arduino_cmake)


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

add_executable(test-ThermoControl EXCLUDE_FROM_ALL
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
	PUBLIC -std=c++17
	PUBLIC -std=gnu++17
	PRIVATE -Wextra
	PRIVATE -Wall
	PRIVATE -Wpedantic
)

################################################################################

# include(esp8266-arduino-cmake/cmake/arduino.cmake)

# add_executable(firmware
# 	src/main.cc
# )

# target_include_directories(firmware
# 	PUBLIC src
# )

# arduino(firmware
# 	ESP8266WiFi
# 	# SD
# 	# SPI
# )

################################################################################

# /home/robert/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.0.4-gcc10.3-1757bed/bin/xtensa-lx106-elf-gcc
# 	-fno-exceptions
# 	-Wl,-Map
# 	-Wl,/tmp/arduino_build_643540/ThermoControl.ino.map
# 	-g
# 	-w
# 	-Werror=return-type
# 	-Os
# 	-nostdlib
# 	-Wl,--no-check-sections
# 	-u app_entry
# 	-u _printf_float
# 	-u _scanf_float
# 	-Wl,-static
# 	-L/home/robert/.arduino15/packages/esp8266/hardware/esp8266/3.0.2/tools/sdk/lib
# 	-L/home/robert/.arduino15/packages/esp8266/hardware/esp8266/3.0.2/tools/sdk/lib/NONOSDK22x_190703
# 	-L/tmp/arduino_build_643540
# 	-L/home/robert/.arduino15/packages/esp8266/hardware/esp8266/3.0.2/tools/sdk/libc/xtensa-lx106-elf/lib
# 	-Tlocal.eagle.flash.ld
# 	-Wl,--gc-sections
# 	-Wl,-wrap,system_restart_local
# 	-Wl,-wrap,spi_flash_read
# 	-o /tmp/arduino_build_643540/ThermoControl.ino.elf
# 	-Wl,--start-group /tmp/arduino_build_643540/sketch/ThermoControl.ino.cpp.o

# 	/tmp/arduino_build_643540/libraries/ThermoControl/___spi_comm.c.o
# 	/tmp/arduino_build_643540/libraries/ArduinoWebsockets/crypto.cpp.o
# 	/tmp/arduino_build_643540/libraries/ArduinoWebsockets/message.cpp.o
# 	/tmp/arduino_build_643540/libraries/ArduinoWebsockets/websockets_client.cpp.o
# 	/tmp/arduino_build_643540/libraries/ArduinoWebsockets/websockets_endpoint.cpp.o
# 	/tmp/arduino_build_643540/libraries/ArduinoWebsockets/websockets_server.cpp.o
# 	/tmp/arduino_build_643540/libraries/ArduinoWebsockets/ws_common.cpp.o
# 	/tmp/arduino_build_643540/libraries/ESP8266WiFi/ESP8266WiFi.a
# 	/tmp/arduino_build_643540/libraries/SPI/SPI.cpp.o
# 	/tmp/arduino_build_643540/libraries/SD/SD.a
# 	/tmp/arduino_build_643540/libraries/SDFS/SDFS.a
# 	/tmp/arduino_build_643540/libraries/ESP8266SdFat/FreeStack.cpp.o
# 	/tmp/arduino_build_643540/libraries/ESP8266SdFat/MinimumSerial.cpp.o
# 	/tmp/arduino_build_643540/libraries/ESP8266SdFat/ExFatLib/ExFatDbg.cpp.o
# 	/tmp/arduino_build_643540/libraries/ESP8266SdFat/ExFatLib/ExFatFile.cpp.o
# 	/tmp/arduino_build_643540/libraries/ESP8266SdFat/ExFatLib/ExFatFilePrint.cpp.o
# 	/tmp/arduino_build_643540/libraries/ESP8266SdFat/ExFatLib/ExFatFileWrite.cpp.o
# 	/tmp/arduino_build_643540/libraries/ESP8266SdFat/ExFatLib/ExFatFormatter.cpp.o
# 	/tmp/arduino_build_643540/libraries/ESP8266SdFat/ExFatLib/ExFatPartition.cpp.o
# 	/tmp/arduino_build_643540/libraries/ESP8266SdFat/ExFatLib/ExFatVolume.cpp.o
# 	/tmp/arduino_build_643540/libraries/ESP8266SdFat/ExFatLib/upcase.cpp.o
# 	/tmp/arduino_build_643540/libraries/ESP8266SdFat/FatLib/FatDbg.cpp.o
# 	/tmp/arduino_build_643540/libraries/ESP8266SdFat/FatLib/FatFile.cpp.o
# 	/tmp/arduino_build_643540/libraries/ESP8266SdFat/FatLib/FatFileLFN.cpp.o
# 	/tmp/arduino_build_643540/libraries/ESP8266SdFat/FatLib/FatFilePrint.cpp.o
# 	/tmp/arduino_build_643540/libraries/ESP8266SdFat/FatLib/FatFileSFN.cpp.o
# 	/tmp/arduino_build_643540/libraries/ESP8266SdFat/FatLib/FatFormatter.cpp.o
# 	/tmp/arduino_build_643540/libraries/ESP8266SdFat/FatLib/FatPartition.cpp.o
# 	/tmp/arduino_build_643540/libraries/ESP8266SdFat/FatLib/FatVolume.cpp.o
# 	/tmp/arduino_build_643540/libraries/ESP8266SdFat/FsLib/FsFile.cpp.o
# 	/tmp/arduino_build_643540/libraries/ESP8266SdFat/FsLib/FsNew.cpp.o
# 	/tmp/arduino_build_643540/libraries/ESP8266SdFat/FsLib/FsVolume.cpp.o
# 	/tmp/arduino_build_643540/libraries/ESP8266SdFat/SdCard/SdCardInfo.cpp.o
# 	/tmp/arduino_build_643540/libraries/ESP8266SdFat/SdCard/SdSpiCard.cpp.o
# 	/tmp/arduino_build_643540/libraries/ESP8266SdFat/SdCard/SdioTeensy.cpp.o
# 	/tmp/arduino_build_643540/libraries/ESP8266SdFat/SpiDriver/SdSpiArtemis.cpp.o
# 	/tmp/arduino_build_643540/libraries/ESP8266SdFat/SpiDriver/SdSpiChipSelect.cpp.o
# 	/tmp/arduino_build_643540/libraries/ESP8266SdFat/SpiDriver/SdSpiDue.cpp.o
# 	/tmp/arduino_build_643540/libraries/ESP8266SdFat/SpiDriver/SdSpiESP.cpp.o
# 	/tmp/arduino_build_643540/libraries/ESP8266SdFat/SpiDriver/SdSpiParticle.cpp.o
# 	/tmp/arduino_build_643540/libraries/ESP8266SdFat/SpiDriver/SdSpiSTM32.cpp.o
# 	/tmp/arduino_build_643540/libraries/ESP8266SdFat/SpiDriver/SdSpiTeensy3.cpp.o
# 	/tmp/arduino_build_643540/libraries/ESP8266SdFat/common/FmtNumber.cpp.o
# 	/tmp/arduino_build_643540/libraries/ESP8266SdFat/common/FsDateTime.cpp.o
# 	/tmp/arduino_build_643540/libraries/ESP8266SdFat/common/FsStructs.cpp.o
# 	/tmp/arduino_build_643540/libraries/ESP8266SdFat/common/PrintBasic.cpp.o
# 	/tmp/arduino_build_643540/libraries/ESP8266SdFat/common/SysCallBareUno.cpp.o
# 	/tmp/arduino_build_643540/libraries/ESP8266SdFat/iostream/StdioStream.cpp.o
# 	/tmp/arduino_build_643540/libraries/ESP8266SdFat/iostream/StreamBaseClass.cpp.o
# 	/tmp/arduino_build_643540/libraries/ESP8266SdFat/iostream/istream.cpp.o
# 	/tmp/arduino_build_643540/libraries/ESP8266SdFat/iostream/ostream.cpp.o
# 	/tmp/arduino_build_643540/core/core.a

# 	-lhal
# 	-lphy
# 	-lpp
# 	-lnet80211
# 	-llwip2-536-feat
# 	-lwpa
# 	-lcrypto
# 	-lmain
# 	-lwps
# 	-lbearssl
# 	-lespnow
# 	-lsmartconfig
# 	-lairkiss
# 	-lwpa2
# 	-lstdc++
# 	-lm
# 	-lc
# 	-lgcc
# 	-Wl,--end-group

# 	-L/tmp/arduino_build_643540

# add_executable(ThermoControl
# 	src/system.h)



################################################################################
#upload####
###########

#/home/robert/.arduino15/packages/esp8266/tools/python3/3.7.2-post1/python3
#	-I /home/robert/.arduino15/packages/esp8266/hardware/esp8266/3.0.2/tools/upload.py
# --chip esp8266
# --port /dev/ttyUSB0
# --baud 115200
# --before default_reset
# --after hard_reset write_flash 0x0
# /tmp/arduino_build_643540/ThermoControl.ino.bin


