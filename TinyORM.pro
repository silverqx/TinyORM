TEMPLATE = subdirs

# Version requirements
# ---
# Older versions may work, but you are on your own
# Req - requirement, throws error
# Rec - recommended, shows info message

# 16.10/16.11 (1929) - to support #pragma system_header
tinyMinReqMsvc    = 19.29
tinyMinReqClangCl = 14.0.3
tinyMinRecClang   = 12
tinyMinRecGCC     = 10.2
tinyMinRecQt      = 5.15.2

# Make minimum toolchain version a requirement
load(tiny_toolchain_requirement)
# The staticlib option check
load(tiny_staticlib_check)

# Subprojects
# ---

load(tiny_drivers)
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
