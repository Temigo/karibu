CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(OpenCV_DIR "/usr/local/share/OpenCV")
message("OpenCV_DIR : " ${OpenCV_DIR})

set(X11_INCLUDE_DIR "/usr/include/X11")

FIND_PACKAGE(OpenCV REQUIRED)
INCLUDE_DIRECTORIES(${X11_INCLUDE_DIR})

if(OpenCV_FOUND)
   message("Found OpenCV")
   message("\t Includes: " ${OpenCV_INCLUDE_DIRS})
   message("\t Libs : " ${OpenCV_LIBS})
   message("\t Version: " ${OpenCV_VERSION})
endif(OpenCV_FOUND)

if(X11_FOUND)
    message("Found X11")
endif(X11_FOUND)

PROJECT(karibu)

ADD_EXECUTABLE(karibu main.cpp hand.cpp actions.cpp utils.cpp)
TARGET_LINK_LIBRARIES(karibu ${OpenCV_LIBS} X11)
