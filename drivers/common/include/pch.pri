# Use Precompiled headers (PCH)
# ---

PRECOMPILED_HEADER = $$quote($$PWD/pch.h)
HEADERS += $$PRECOMPILED_HEADER

precompile_header: \
    DEFINES *= TINYDRIVERS_USING_PCH
