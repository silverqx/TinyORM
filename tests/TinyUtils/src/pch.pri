# Use Precompiled headers (PCH)
# ---

PRECOMPILED_HEADER = $$quote($$PWD/pch.h)
HEADERS += $$PRECOMPILED_HEADER

precompile_header: \
    DEFINES *= TINYORM_USING_PCH
