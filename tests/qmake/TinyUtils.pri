# TinyUtils library defines
# ---

CONFIG(shared, dll|shared|static|staticlib) | \
CONFIG(dll, dll|shared|static|staticlib): \
    DEFINES *= TINYUTILS_LINKING_SHARED

# TinyUtils library headers include path
# ---

INCLUDEPATH += $$quote($$TINYORM_SOURCE_TREE/tests/TinyUtils/src/)

# Link against tests's TinyUtils library
# ---

LIBS += $$quote(-L$$TINYORM_BUILD_TREE/tests/TinyUtils$$TINY_BUILD_SUBFOLDER/)
LIBS += -lTinyUtils
