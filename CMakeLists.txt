CMAKE_MINIMUM_REQUIRED(VERSION 2.6)


set(OpenCV_DIR "/usr/local/share/OpenCV")
message("OpenCV_DIR : " ${OpenCV_DIR})

set(X11_INCLUDE_DIR "/usr/include/X11")

FIND_PACKAGE(OpenCV REQUIRED)
#FIND_PACKAGE(X11 REQUIRED)
INCLUDE_DIRECTORIES(${X11_INCLUDE_DIR})

if(OpenCV_FOUND)
   message("Found OpenCV")
   message("Includes: " ${OpenCV_INCLUDE_DIRS})
   message("Libs : " ${OpenCV_LIBS})
   message("Version: " ${OpenCV_VERSION})
endif(OpenCV_FOUND)

if(X11_FOUND)
    message("Found X11")
endif(X11_FOUND)

PROJECT(karibu)

ADD_EXECUTABLE(karibu main.cpp hand.cpp actions.cpp)
TARGET_LINK_LIBRARIES(karibu ${OpenCV_LIBS} X11)
#TARGET_LINK_LIBRARIES(karibu ${X11_INCLUDE_DIR})
