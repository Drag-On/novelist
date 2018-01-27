
# ============================================================== #
# Print configuration summary
# ============================================================== #
function(print_config_summary)
    print_status( "********** Configuration Summary **********")
    print_status( "General:")
    print_status( "  System            :   ${CMAKE_SYSTEM_NAME}")
    print_status( "  C++ compiler      :   ${CMAKE_CXX_COMPILER}")
    print_status( "  Release CXX flags :   ${CMAKE_CXX_FLAGS_RELEASE} ${CMAKE_CXX_FLAGS}")
    print_status( "  Debug CXX flags   :   ${CMAKE_CXX_FLAGS_DEBUG} ${CMAKE_CXX_FLAGS}")
    print_status( "  Build type        :   ${CMAKE_BUILD_TYPE}")
    print_status( "")
    print_status( "Dependencies:")
    print_status( "  Qt5               :   ${Qt5_VERSION}")
    print_status( "  Catch             :   ${CATCH_VERSION}")
    print_status( "*******************************************")
endfunction()