# Common for static/shared libraries
# ---

CONFIG *= create_prl create_pc create_libtool

# Common for static libraries
# ---

win32: \
if(CONFIG(static, dll|shared|static|staticlib) | \
CONFIG(staticlib, dll|shared|static|staticlib)): \
    CONFIG -= embed_manifest_dll
