cmake_minimum_required(VERSION 3.16)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${CMAKE_SOURCE_DIR}/include)

find_package(OpenSSL REQUIRED)


add_executable(blockchain
  src/main.c
  src/blockchain.c
)

#amd64
set(CHECK_LIB "${CMAKE_SOURCE_DIR}/lib/libcheck-amd64.so")

target_link_libraries(blockchain PRIVATE OpenSSL::Crypto "${CHECK_LIB}")

# Runtime search path for the .so
set_target_properties(blockchain PROPERTIES
  BUILD_RPATH "$ORIGIN/../lib;$ORIGIN/lib;$ORIGIN"
  INSTALL_RPATH "$ORIGIN/../lib;$ORIGIN/lib;$ORIGIN"
)

