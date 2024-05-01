# Common for static/shared libraries (don't use for loadable modules libraries)
# ---

CONFIG *= create_prl create_pc create_libtool

# Common for static libraries
# ---

win32: \
if(CONFIG(static, dll|shared|static|staticlib) | \
CONFIG(staticlib, dll|shared|static|staticlib)): \
    CONFIG -= embed_manifest_dll

# Common defines for static/shared libraries
# ---

# To disable #pragma system_header if compiling TinyORM project itself
DEFINES *= TINYORM_PRAGMA_SYSTEM_HEADER_OFF
