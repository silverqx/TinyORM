TEMPLATE = subdirs

# Version requirements
# ---
# Older versions may work, but you are on your own
# Req - requirement, throws error
# Rec - recommended, shows info message

# 16.10/16.11 (1929) - to support #pragma system_header
tinyMinReqMsvc    = 19.29
tinyMinReqQt      = 6.4
tinyMinReqClangCl = 14.0.3
tinyMinRecClang   = 12
tinyMinRecGCC     = 10.2
# Unused
#tinyMinReqRangeV3Version  = 0.12.0
#tinyMinReqTabulateVersion = 1.5.0

# Check/verify all toolchain requirements and edge cases, throws error() on any problem
load(private/tiny_toolchain_requirement)
# The staticlib option check
load(private/tiny_staticlib_check)

# Sub-projects
# ---

load(private/tiny_drivers)
tiny_drivers_check_build_types()

SUBDIRS =

# Can be enabled using the CONFIG += build_shared/loadable/static_drivers
tiny_is_building_drivers(): \
    SUBDIRS += drivers

SUBDIRS += src

tiny_is_building_drivers(): \
    src.depends = drivers

# Can be enabled using the CONFIG += tom_example
tom_example {
    SUBDIRS += examples
    examples.depends = src
}

# Can be enabled using the CONFIG += build_tests
build_tests {
    SUBDIRS += tests
    tests.depends = src

    !build_pass: message( "Build TinyORM unit tests." )
}
