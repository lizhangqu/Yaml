project(YAML)
cmake_minimum_required (VERSION 3.6)

#remove debug symbol when release
if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -s")
endif()

#if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
# add_definitions(DENABLE_LOG)
#endif()

include_directories(
  ${PROJECT_SOURCE_DIR}/include/ #jni source
  ${PROJECT_SOURCE_DIR}/../../../../thirdparty/yaml-cpp/include/ #yaml-cpp
  /usr/local/include #boost
  )

# boost install
# brew update
# brew install boost
# brew upgrade boost

file(GLOB YAML_SOURCES
  ${PROJECT_SOURCE_DIR}/../../../../thirdparty/yaml-cpp/src/*.cpp #yamp-cpp
)

add_library(yaml-cpp-static STATIC ${YAML_SOURCES})

file(GLOB JNI_SOURCES
  ${PROJECT_SOURCE_DIR}/*.cpp #jni source
  ${PROJECT_SOURCE_DIR}/../../../../thirdparty/boost/filesystem/src/*.cpp #boost
  ${PROJECT_SOURCE_DIR}/../../../../thirdparty/boost/system/src/*.cpp #boost
)

add_library(yaml SHARED ${JNI_SOURCES})

target_link_libraries(yaml
  yaml-cpp-static
  log
  )