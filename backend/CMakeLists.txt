cmake_minimum_required(VERSION 3.20)

project(EncryptionAPI VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Find native MSYS2 packages automatically
find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)
find_package(Threads REQUIRED)

file(GLOB_RECURSE SOURCES
    src/*.c
)

list(APPEND SOURCES
    external/mongoose/mongoose.c
    external/cJSON/cJSON.c
)

add_executable(EncryptionAPI ${SOURCES})

target_include_directories(EncryptionAPI PRIVATE
    include
    external/mongoose
    external/cJSON
)

target_link_libraries(EncryptionAPI PRIVATE 
    OpenSSL::SSL 
    OpenSSL::Crypto
    CURL::libcurl
    Threads::Threads
)

if(WIN32)
    target_link_libraries(EncryptionAPI PRIVATE ws2_32 crypt32 wsock32)
endif()