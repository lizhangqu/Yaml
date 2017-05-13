project(YAML)
cmake_minimum_required (VERSION 3.6)

#remove debug symbol when release
if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -s")
endif()

file(GLOB YAML_SOURCES ${PROJECT_SOURCE_DIR}/../../../../yaml-cpp/src/*.cpp)

include_directories(
  ${PROJECT_SOURCE_DIR}/../../../../yaml-cpp/include/
  )

add_library(yaml-cpp-static STATIC ${YAML_SOURCES})


include_directories(
  ${PROJECT_SOURCE_DIR}/include/
  )

file(GLOB_RECURSE YAML_JNI_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)
add_library(yaml SHARED ${YAML_JNI_SOURCES})

target_link_libraries(yaml
  yaml-cpp-static
  log
  )