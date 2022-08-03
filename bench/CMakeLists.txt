cmake_minimum_required(VERSION 3.16)

add_executable(main main.cpp)

target_include_directories(main PRIVATE ${PROJECT_SOURCE_DIR}/include)

if(WIN32)
  target_link_libraries(main PRIVATE winmm)
  target_compile_definitions(main PRIVATE NOMINMAX)
else()
  find_package(Threads REQUIRED)
  target_link_libraries(main PRIVATE ${CMAKE_THREAD_LIBS_INIT})
endif()
