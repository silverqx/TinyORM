# Helper function for coupling option() and add_feature_info()
function(feature_option name description default)

    string(CONCAT desc "${description} (default: ${default})")

    option(${name} "${desc}" "${default}")

    add_feature_info(${name} ${name} "${desc}")

endfunction()

include(CMakeDependentOption)
# Helper function for coupling cmake_dependent_option() and add_feature_info()
macro(feature_option_dependent name description default depends force)

    string(CONCAT desc
        "${description} (default: ${default}; depends on condition: ${depends})")

    CMAKE_DEPENDENT_OPTION(${name} "${desc}" "${default}" "${depends}" "${force}")

    add_feature_info(${name} ${name} "${desc}")

endmacro()
