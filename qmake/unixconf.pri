# Treat Qt's headers as system headers
QMAKE_CXXFLAGS += -isystem $$shell_quote($$[QT_INSTALL_HEADERS]/)
for(module, QT) {
    equals(module, "testlib"): \
        QMAKE_CXXFLAGS += -isystem $$shell_quote($$[QT_INSTALL_HEADERS]/QtTest/)

    else {
        # Capitalize a first letter, result: -isystem .../include/QtCore/
        moduleList = $$split(module, )
        QMAKE_CXXFLAGS += \
            -isystem $$shell_quote($$[QT_INSTALL_HEADERS]/Qt$$upper(\
                     $$take_first(moduleList))$$join(moduleList, )/)
    }
}
unset(moduleList)

QMAKE_CXXFLAGS_WARN_ON *= \
    -Wall \
    -Wextra \
    -Weffc++ \
    -Werror \
    -Wfatal-errors \
    -Wcast-qual \
    -Wcast-align \
    -Woverloaded-virtual \
    -Wold-style-cast \
    -Wshadow \
    -Wundef \
    -Wfloat-equal \
    -Wformat-security \
    -Wdouble-promotion \
    -Wconversion \
    -Wzero-as-null-pointer-constant \
    -Winvalid-pch \
    -pedantic \
    -pedantic-errors \

# Clang 12 still doesn't support -Wstrict-null-sentinel
!clang: QMAKE_CXXFLAGS_WARN_ON *= -Wstrict-null-sentinel
