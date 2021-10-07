# TinyORM library defines
# ---

CONFIG(shared, dll|shared|static|staticlib) | \
CONFIG(dll, dll|shared|static|staticlib): \
    DEFINES += TINYORM_LINKING_SHARED

# TinyORM library headers include path
# ---

include($$TINYORM_SOURCE_TREE/include/include.pri)

# Link against TinyORM library
# ---

LIBS += $$quote(-L$$TINYORM_BUILD_TREE/src$$TINY_RELEASE_TYPE/)
LIBS += -lTinyOrm
