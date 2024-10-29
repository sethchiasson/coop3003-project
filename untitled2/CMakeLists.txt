cmake_minimum_required(VERSION 3.27)
project(untitled2)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)

# Set the path to libcurl's include and library directories
set(CURL_INCLUDE_DIR "C:/Users/Seth/Downloads/curl-8.10.1_7-win64-mingw/include")
set(CURL_LIBRARIES "C:/Users/Seth/Downloads/curl-8.10.1_7-win64-mingw/lib/libcurl.dll.a")

# Include the libcurl headers directory
include_directories(${CURL_INCLUDE_DIR})

# Define the executable
add_executable(untitled2 main.cpp)

# Link libcurl to the executable
target_link_libraries(untitled2 ${CURL_LIBRARIES})
