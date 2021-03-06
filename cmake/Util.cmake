
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
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        if (MSVC_VERSION GREATER_EQUAL "1900")
            include(CheckCXXCompilerFlag)
            CHECK_CXX_COMPILER_FLAG("/std:c++latest" _cpp_latest_flag_supported)
            if (_cpp_latest_flag_supported)
                target_compile_options(${target} PRIVATE "/std:c++latest")
            else()
                print_status("This version of MSVC doesn't support C++17.")
            endif ()
        else()
            print_status("This version of MSVC is too old.")
        endif()
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

# ============================================================== #
# Enable qt internationalization tools for target
# ============================================================== #
function(enable_i18n target)
    set(TRANSLATION_FILES
            translations/${target}_en.ts
            translations/${target}_de.ts
            )

    option(UPDATE_TRANSLATIONS "Update source translation files")
    if(UPDATE_TRANSLATIONS)
        qt5_create_translation(${target}_QM_FILES src/ include/ ${TRANSLATION_FILES})
    else(UPDATE_TRANSLATIONS)
        qt5_add_translation(${target}_QM_FILES ${TRANSLATION_FILES})
    endif(UPDATE_TRANSLATIONS)
    add_custom_target(${target}_translations ALL DEPENDS ${${target}_QM_FILES})
    install(FILES ${${target}_QM_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/translations)
    set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM ON)
endfunction()