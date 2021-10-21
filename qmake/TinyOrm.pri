# TinyORM configuration
# ---

CONFIG *= c++2a strict_c++ warn_on utf8_source link_prl hide_symbols silent

# Use extern constants for shared build
CONFIG(shared, dll|shared|static|staticlib) | \
CONFIG(dll, dll|shared|static|staticlib): \
    # Support override because inline_constants can be used in the shared build too
    !inline_constants: \
        CONFIG *= extern_constants

# Archive library build
else: \
    CONFIG += inline_constants

# TinyORM defines
# ---

# Link with the shared library
CONFIG(shared, dll|shared|static|staticlib) | \
CONFIG(dll, dll|shared|static|staticlib): \
    DEFINES *= TINYORM_LINKING_SHARED

# Release build
CONFIG(release, debug|release): \
    DEFINES *= TINYORM_NO_DEBUG

# Log queries with a time measurement in debug build
CONFIG(release, debug|release): \
    DEFINES *= TINYORM_NO_DEBUG_SQL
