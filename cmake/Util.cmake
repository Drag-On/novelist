
# ============================================================== #
# Print a status message
# ============================================================== #
function(print_status str)
    message(STATUS ${str})
endfunction()

# ============================================================== #
# Enable C++17 for a target
# ============================================================== #
function(enable_cxx17 target)
    # Cmake 3.8 doesn't support this for MSVC, so do it manually
    # At least Visual Studio 2017 can be supported this way
    if (MSVC_VERSION GREATER_EQUAL "1900")
        include(CheckCXXCompilerFlag)
        CHECK_CXX_COMPILER_FLAG("/std:c++latest" _cpp_latest_flag_supported)
        if (_cpp_latest_flag_supported)
            add_compile_options("/std:c++latest")
        endif ()
    # Try the usual CMake way of adding the standard for other compilers
    else ()
        set_target_properties(${target}
                PROPERTIES
                CXX_STANDARD 17
                CXX_STANDARD_REQUIRED YES
                CXX_EXTENSIONS NO
                )
    endif ()
endfunction()