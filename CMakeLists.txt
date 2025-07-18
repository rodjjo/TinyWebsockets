cmake_minimum_required(VERSION 3.10)
set (CMAKE_CXX_STANDARD 14)

project(tiny_websockets_demo C CXX)

add_subdirectory(tiny_websockets_lib)

if (TEST_ENABLED)
  add_subdirectory(tests)
  enable_testing()
endif()


if(MSVC)
  # Force to always compile with W4
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  # Update if necessary
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -Wextra -pedantic -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-promo -Wstrict-null-sentinel -Wundef -fdiagnostics-show-option")
endif()

# OpenSSL
find_package(OpenSSL REQUIRED)
message("OpenSSL include dir: ${OPENSSL_INCLUDE_DIR}")
message("OpenSSL libraries: ${OPENSSL_LIBRARIES}")
include_directories(${OPENSSL_INCLUDE_DIR})
list(APPEND LIB_LIST ${OPENSSL_LIBRARIES})

if (COMPILE_DEMOS)
  file(GLOB TEST_SOURCES "demo_*.cpp")
  foreach(file ${TEST_SOURCES})
    get_filename_component(_F_NAME ${file} NAME_WE)
    add_executable(${_F_NAME} ${file})
    target_link_libraries (${_F_NAME} ${CMAKE_THREAD_LIBS_INIT})
    target_link_libraries(${_F_NAME} tiny_websockets_lib)
    if(WIN32)
      target_link_libraries(${_F_NAME} wsock32 ws2_32)
    endif()
    install(TARGETS ${_F_NAME} DESTINATION bin)
  endforeach()
endif()