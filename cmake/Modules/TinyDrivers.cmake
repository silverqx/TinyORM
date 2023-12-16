# Initialize TinyDrivers CMake internal cache variables, early initialization
function(tiny_init_driver_types_pre)

    set(driversTypeForce DRIVERS_TYPE-NOTFOUND)

    # Build the TinyDrivers library as a static archive if the TinyOrm library is built
    # as a static archive; also, the DRIVERS_TYPE option will be hidden in this case
    # because it can't have any other value
    if(BUILD_DRIVERS AND NOT BUILD_SHARED_LIBS)
        set(driversTypeForce Static)
    endif()

    set(TINY_DRIVERS_TYPE_FORCE ${driversTypeForce} CACHE INTERNAL
        "Specifies value for the 'force' parameter \
for the feature_string_option_dependent(DRIVERS_TYPE) function")

endfunction()

# Initialize TinyDrivers build types CMake internal cache variables (for nicer if()-s)
function(tiny_init_driver_types)

    # Initialize to undefined values instead of OFF if TinyDrivers are not built
    if(NOT BUILD_DRIVERS)
        set(driversShared      TINY_BUILD_SHARED_DRIVERS-NOTFOUND)
        set(driversStatic      TINY_BUILD_STATIC_DRIVERS-NOTFOUND)
        set(driversLoadable    TINY_BUILD_LOADABLE_DRIVERS-NOTFOUND)
        set(driversLibraryType TINY_BUILD_LOADABLE_DRIVERS-NOTFOUND)
    else()
        set(driversShared   OFF)
        set(driversStatic   OFF)
        set(driversLoadable OFF)
        # TinyDrivers library type for the add_library() CMake function
        string(TOUPPER "${DRIVERS_TYPE}" driversLibraryType)
        # For case-insensitive comparisons
        string(TOLOWER "${DRIVERS_TYPE}" driversTypeLower)

        # Build as one TinyDrivers shared library with all enabled SQL drivers inside
        if(driversTypeLower STREQUAL "shared")
            set(driversShared ON)

        # Build TinyDrivers as a static archive library (will be linked into the TinyOrm
        # shared or static libary)
        elseif(driversTypeLower STREQUAL "static")
            set(driversStatic ON)

        # Build all enabled SQL drivers as loadable shared libraries which will be loaded
        # at runtime by the TinyDrivers shared library
        elseif(driversTypeLower STREQUAL "loadable")
            set(driversLoadable ON)
            # TinyDrivers library must be of the SHARED type if building SQL drivers
            # as loadable shared libraries
            set(driversLibraryType "SHARED")

        else()
            message(FATAL_ERROR "Unsupported value '${DRIVERS_TYPE}' for
the DRIVERS_TYPE CMake option for ${TinyDrivers_target} library, allowed values are \
Shared, Static, or Loadable (case-insensitive).")
        endif()

        message(VERBOSE "Building ${TinyDrivers_target} ${driversTypeLower} library")
    endif()

    set(TINY_BUILD_SHARED_DRIVERS ${driversShared} CACHE INTERNAL
        "Determine whether ${TinyDrivers_target} library will be built as a shared \
library with all enabled SQL drivers inside")
    set(TINY_BUILD_STATIC_DRIVERS ${driversStatic} CACHE INTERNAL
        "Determine whether ${TinyDrivers_target} library will be built as a static \
library that will be linked into the ${TinyOrm_target} shared or static library")
    set(TINY_BUILD_LOADABLE_DRIVERS ${driversLoadable} CACHE INTERNAL
        "Determine whether all enabled SQL drivers will be built as loadable shared \
libraries which will be loaded at runtime by the ${TinyDrivers_target} shared library")
    set(TINY_DRIVERS_LIBRARY_TYPE ${driversLibraryType} CACHE INTERNAL
        "Specifies ${TinyDrivers_target} library type for the add_library() CMake \
function")

endfunction()
