# Treat Qt's headers as system headers
QMAKE_CXXFLAGS += -isystem "$$[QT_INSTALL_HEADERS]"
for(module, QT) {
    equals(module, "testlib") {
        QMAKE_CXXFLAGS += -isystem $$quote($$[QT_INSTALL_HEADERS]/QtTest)
    } else {
        QMAKE_CXXFLAGS += \
            -isystem "$$[QT_INSTALL_HEADERS]/Qt$$system("echo $$module |\
                     sed 's/.*/\u&/'")"
    }
}

QMAKE_CXXFLAGS_WARN_ON *= \
    -Wall \
    -Wextra \
    -Wcast-qual \
    -Wcast-align \
    -Woverloaded-virtual \
    -Wold-style-cast \
    -Wnon-virtual-dtor \
    -Wshadow \
    -Wundef \
    -Wfloat-equal \
    -Wformat-security \
    -Wdouble-promotion \
    -Wconversion \
    -Wzero-as-null-pointer-constant \
    -Wstrict-null-sentinel \
    -Winvalid-pch \
    -pedantic \
    -pedantic-errors \
