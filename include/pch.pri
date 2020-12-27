# Use Precompiled headers (PCH)
# ---

PRECOMPILED_HEADER = $$quote($$PWD/pch.h)

precompile_header:!isEmpty(PRECOMPILED_HEADER) {
    DEFINES += USING_PCH
}

HEADERS += $$quote($$PWD/pch.h)
