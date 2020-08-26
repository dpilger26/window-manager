cmake_minimum_required(VERSION 3.14)

project("window-manager-app" CXX)

find_package(yaml-cpp REQUIRED)

add_executable(${PROJECT_NAME} 
  Config.hpp
  Controls.hpp
  Error.hpp
  Keys.hpp
  main.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
  ${YAML_CPP_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  ${YAML_CPP_LIBRARIES}
)

set_target_properties(${PROJECT_NAME}
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../bin/$<0:>
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
  DEBUG
)

target_compile_options(${PROJECT_NAME} PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4>
  $<$<CXX_COMPILER_ID:MSVC>:/Zi>
  $<$<CXX_COMPILER_ID:MSVC>:/sdl>
  $<$<CXX_COMPILER_ID:MSVC>:/MP>
  $<$<CXX_COMPILER_ID:MSVC>:/Gy>
  $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/Oi>
  $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/Ot>
  $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/GL>
)

target_link_options(${PROJECT_NAME} PRIVATE
  $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/LTCG>
)
