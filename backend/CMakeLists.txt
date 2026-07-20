cmake_minimum_required(VERSION 3.20)

project(EncryptionAPI VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# ==============================================================================
# NOTE ON OPENSSL PATH CONFIGURATION:
# CMake needs to locate the OpenSSL headers and development libraries.
# If CMake cannot find OpenSSL automatically, uncomment the block below 
# and adjust the path to match where your OpenSSL installation lives.
# Examples:
#   - MSYS2 UCRT64: "C:/msys64/ucrt64"
#   - Shining Light Productions: "C:/Program Files/OpenSSL-Win64"
# ==============================================================================
if(WIN32)
    set(OPENSSL_ROOT_DIR "C:/msys64/ucrt64" CACHE PATH "Path to OpenSSL root directory")
endif()

# 1. Find the OpenSSL package
find_package(OpenSSL REQUIRED)

file(GLOB_RECURSE SOURCES
    src/*.c
)

list(APPEND SOURCES
    external/mongoose/mongoose.c
    external/cJSON/cJSON.c
)

add_executable(EncryptionAPI ${SOURCES})

# Modern target-based include directories
target_include_directories(EncryptionAPI PRIVATE
    include
    external/mongoose
    external/cJSON
    ${OPENSSL_INCLUDE_DIR}
)

# 2. Link OpenSSL and its dependencies to your executable
target_link_libraries(EncryptionAPI PRIVATE OpenSSL::SSL OpenSSL::Crypto)

if(WIN32)
    target_link_libraries(EncryptionAPI PRIVATE ws2_32 crypt32)
endif()