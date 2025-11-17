# tests/CMakeLists.txt
cmake_minimum_required(VERSION 3.20)

# Создание тестов
enable_testing()

# Зависимости для тестов
find_package(GTest QUIET)
if(NOT GTest_FOUND)
    # Если GTest не найден, можно использовать подмодуль или установить
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9fd7188aaee5fc40789e4b3.zip
    )
    FetchContent_MakeAvailable(googletest)
endif()

# Тесты для основных компонентов
add_executable(test_handles
    test_handles.cpp
)

add_executable(test_functions
    test_functions.cpp
)

add_executable(test_concepts
    test_concepts.cpp
)

add_executable(test_batch_operations
    test_batch_operations.cpp
)

# Компиляционные определения для тестов
target_compile_definitions(test_handles PRIVATE GL46_ENABLE_VALIDATION)
target_compile_definitions(test_functions PRIVATE GL46_ENABLE_VALIDATION)
target_compile_definitions(test_concepts PRIVATE GL46_ENABLE_VALIDATION)
target_compile_definitions(test_batch_operations PRIVATE GL46_ENABLE_VALIDATION)

# Линковка с библиотекой и GTest
target_link_libraries(test_handles gl46_core GTest::gtest GTest::gtest_main)
target_link_libraries(test_functions gl46_core GTest::gtest GTest::gtest_main)
target_link_libraries(test_concepts gl46_core GTest::gtest GTest::gtest_main)
target_link_libraries(test_batch_operations gl46_core GTest::gtest GTest::gtest_main)

# Добавление тестов
add_test(NAME test_handles_run COMMAND test_handles)
add_test(NAME test_functions_run COMMAND test_functions)
add_test(NAME test_concepts_run COMMAND test_concepts)
add_test(NAME test_batch_operations_run COMMAND test_batch_operations)