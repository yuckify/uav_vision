include_directories(".")

#check for boost regex
find_path(BR_INCLUDE_DIR boost/regex.hpp /usr/include /usr/local/include)
find_library(BR_LIBRARY NAMES boost_regex PATH /usr/lib /usr/local/lib)

if(BR_INCLUDE_DIR AND BR_LIBRARY)
  message(STATUS "Found boost_regex: ${BR_LIBRARY}")
else()
  message(FATAL_ERROR "Could not find boost_regex")
endif()

#setup the build flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++0x -O3")

#grab any files in the src/ folder and compile them
#into the libraries
file(GLOB SRCS "*.cpp")

#build a shared and static link library
add_library(ooSockets_shared SHARED ${SRCS})
add_library(ooSockets_static STATIC ${SRCS})

#make sure the libraries are named as ooSockets[suffix]
set_target_properties(ooSockets_shared ooSockets_static PROPERTIES OUTPUT_NAME ooSockets)

#detect all the header files so they can be installed too
file(GLOB HEADS "*.h")

#install all the files
#get the proper paths to install the files into
find_path(INSTALL_LIB_DIR NAMES lib PATHS /)
find_path(INSTALL_INC_DIR NAMES include PATHS /)
find_path(INSTALL_PDIR NAMES pkgconfig PATHS /usr/lib /usr/share)

set(INSTALL_LIB_DIR "${INSTALL_LIB_DIR}/lib")
set(INSTALL_INC_DIR "${INSTALL_INC_DIR}/include/ooSockets")
set(INSTALL_PDIR "${INSTALL_PDIR}/pkgconfig")

file(WRITE "ooSockets.pc" "Name: ooSockets\n")
file(APPEND "ooSockets.pc" "Description: Simple object oriented socket library\n")
file(APPEND "ooSockets.pc" "Version: 1.0.0\n")
file(APPEND "ooSockets.pc" "Libs: --std=c++0x -L${INSTALL_LIB_DIR} -looSockets -lpthread -lboost_regex\n")
file(APPEND "ooSockets.pc" "Cflags: -I${INSTALL_INC_DIR}\n")

#do the actual installation
install(TARGETS ooSockets_shared ooSockets_static 
  DESTINATION lib)
install(FILES ${HEADS} DESTINATION "${INSTALL_INC_DIR}")
install(FILES "ooSockets.pc" DESTINATION "${INSTALL_PDIR}")

#this is for the make uninstall command
CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
