cmake_minimum_required(VERSION 3.12)

project(epinyin)

find_package(Boost REQUIRED)
find_package(Catch2)

add_executable(epinyin test_syllable_segmentation.cpp)
target_link_libraries(epinyin PRIVATE ${Boost_LIBRARIES})
target_include_directories(epinyin PRIVATE ${Boost_INCLUDE_DIRS})
target_compile_features(epinyin PUBLIC cxx_std_17)
