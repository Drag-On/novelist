

add_subdirectory(GSL)
set_target_properties(catch PROPERTIES EXCLUDE_FROM_ALL TRUE)
set_target_properties(test_catch PROPERTIES EXCLUDE_FROM_ALL TRUE)
set_target_properties(span_tests PROPERTIES EXCLUDE_FROM_ALL TRUE)
set_target_properties(multi_span_tests PROPERTIES EXCLUDE_FROM_ALL TRUE)
set_target_properties(strided_span_tests PROPERTIES EXCLUDE_FROM_ALL TRUE)
set_target_properties(string_span_tests PROPERTIES EXCLUDE_FROM_ALL TRUE)
set_target_properties(at_tests PROPERTIES EXCLUDE_FROM_ALL TRUE)
set_target_properties(bounds_tests PROPERTIES EXCLUDE_FROM_ALL TRUE)
set_target_properties(notnull_tests PROPERTIES EXCLUDE_FROM_ALL TRUE)
set_target_properties(assertion_tests PROPERTIES EXCLUDE_FROM_ALL TRUE)
set_target_properties(utils_tests PROPERTIES EXCLUDE_FROM_ALL TRUE)
set_target_properties(owner_tests PROPERTIES EXCLUDE_FROM_ALL TRUE)
set_target_properties(byte_tests PROPERTIES EXCLUDE_FROM_ALL TRUE)
set_target_properties(algorithm_tests PROPERTIES EXCLUDE_FROM_ALL TRUE)

add_subdirectory(libzip)
set_property(TARGET zip PROPERTY POSITION_INDEPENDENT_CODE ON)
target_include_directories(zip
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/libzip/lib/
        ${CMAKE_CURRENT_BINARY_DIR}/libzip/
        )
