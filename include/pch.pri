# Use Precompiled headers (PCH)
# ---

PRECOMPILED_HEADER = $$quote($$PWD/pch.h)
HEADERS += $$PRECOMPILED_HEADER

precompile_header: \
    DEFINES *= USING_PCH
