cmake_minimum_required(VERSION 3.12)

project(epinyin)

find_package(Boost REQUIRED)
find_package(Catch2)
find_package(unofficial-abseil CONFIG REQUIRED)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/externals/sanitizers-cmake/cmake" ${CMAKE_MODULE_PATH})
find_package(Sanitizers)

add_executable(epinyin_test test_syllable_segmentation.cpp)
target_link_libraries(epinyin_test PRIVATE ${Boost_LIBRARIES} unofficial::abseil::base unofficial::abseil::strings)
target_include_directories(epinyin_test PRIVATE ${Boost_INCLUDE_DIRS})
target_compile_features(epinyin_test PUBLIC cxx_std_17)
add_sanitizers(epinyin_test)

add_executable(fuzz_pinyin test_fuzz.cpp)
target_link_libraries(fuzz_pinyin PRIVATE ${Boost_LIBRARIES} unofficial::abseil::base unofficial::abseil::strings)
target_compile_features(fuzz_pinyin PUBLIC cxx_std_17)
target_compile_options(fuzz_pinyin
            PRIVATE $<$<C_COMPILER_ID:Clang>:-g -O1 -fsanitize=fuzzer>
            )

target_link_libraries(fuzz_pinyin
            PRIVATE $<$<C_COMPILER_ID:Clang>:-fsanitize=fuzzer>
            )
