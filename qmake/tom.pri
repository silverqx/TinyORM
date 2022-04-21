TINYORM_SOURCE_TREE = $$clean_path($$quote($$PWD/..))
TINYTOM_SOURCE_TREE = $$quote($$TINYORM_SOURCE_TREE/tom)

# Qt Common Configuration
# ---

QT *= core sql

CONFIG *= link_prl

include($$TINYORM_SOURCE_TREE/qmake/common.pri)

# Configure TinyORM library
# ---
# everything other is defined in the qmake/common.pri

# Link with the shared library
CONFIG(shared, dll|shared|static|staticlib) | \
CONFIG(dll, dll|shared|static|staticlib): \
    DEFINES *= TINYORM_LINKING_SHARED

# File version
# ---

# Find version numbers in the version header file and assign them to the
# <TARGET>_VERSION_<MAJOR,MINOR,PATCH,TWEAK> and also to the VERSION variable.
load(tiny_version_numbers)
tiny_version_numbers()

# Windows resource and manifest files
# ---

# Find version.hpp
tinyRcIncludepath = $$quote($$TINYTOM_SOURCE_TREE/include/)
# Find Windows manifest
mingw: tinyRcIncludepath += $$quote($$TINYTOM_SOURCE_TREE/resources/)

load(tiny_resource_and_manifest)
tiny_resource_and_manifest(                                               \
    $$tinyRcIncludepath, $$TINYTOM_SOURCE_TREE/resources, tom, TomExample \
)

# Link against TinyORM library
# ---

INCLUDEPATH *= \
    $$quote($$TINYORM_SOURCE_TREE/include/) \
    $$quote($$TINYTOM_SOURCE_TREE/include/) \

!isEmpty(TINYORM_BUILD_TREE): \
exists($$TINYORM_BUILD_TREE): {
    LIBS += $$quote(-L$$TINYORM_BUILD_TREE/src$${TINY_RELEASE_TYPE}/)
    LIBS += -lTinyOrm
}
