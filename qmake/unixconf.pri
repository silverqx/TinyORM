# Compiler and Linker options
# ---

QMAKE_CXXFLAGS_WARN_ON *= \
    -Wall \
    -Wextra \
    -Weffc++ \
    -Werror \
    -Wfatal-errors \
    -Winvalid-pch \
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
    -Wuninitialized \
    -pedantic \
    -pedantic-errors \

clang: \
    QMAKE_CXXFLAGS_WARN_ON *= -Wdeprecated
gcc: \
    QMAKE_CXXFLAGS_WARN_ON *= -Wdeprecated-copy-dtor

# Clang 12 still doesn't support -Wstrict-null-sentinel
!clang: QMAKE_CXXFLAGS_WARN_ON *= -Wstrict-null-sentinel

# Allow to enable UBSan with Clang
clang:ubsan {
    QMAKE_CXXFLAGS += -O1

    QMAKE_CXXFLAGS *= \
        -fsanitize=undefined \
        -fsanitize=nullability \
        -fsanitize=float-divide-by-zero \
        -fsanitize=unsigned-integer-overflow \
        -fsanitize=implicit-conversion \
        -fsanitize=local-bounds \

    QMAKE_LFLAGS *= \
        -fsanitize=undefined \
        -fsanitize=nullability \
        -fsanitize=float-divide-by-zero \
        -fsanitize=unsigned-integer-overflow \
        -fsanitize=implicit-conversion \
        -fsanitize=local-bounds
}
