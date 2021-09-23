# WinApi
# ---

# TODO stackoverflow can I pass NTDDI_WIN10_19H1 in /D instead of magic numbers? https://docs.microsoft.com/en-us/windows/win32/winprog/using-the-windows-headers silverqx
# Windows 10 1903 "19H1" - 0x0A000007
DEFINES += NTDDI_VERSION=0x0A000007
# Windows 10 - 0x0A00
DEFINES += _WIN32_WINNT=0x0A00
DEFINES += _WIN32_IE=0x0A00
# Exclude unneeded header files
DEFINES += WIN32_LEAN_AND_MEAN
DEFINES += NOMINMAX

# Compiler and Linker options
# ---

win32-msvc {
    # I don't use -MP flag, because using jom
    # strict-c++ does not enable -permissive- on MSVC
    QMAKE_CXXFLAGS += -guard:cf -bigobj -permissive- -Zc:ternary
    QMAKE_CXXFLAGS_DEBUG += -RTC1
    QMAKE_CXXFLAGS_WARN_ON = -external:anglebrackets -external:W0 -WX -W4 -wd4702
    QMAKE_LFLAGS += /guard:cf /WX
    QMAKE_LFLAGS_RELEASE += /OPT:REF,ICF=5
}

win32-clang-g++ {
    # -mthreads is unused on Clang
    QMAKE_CXXFLAGS_EXCEPTIONS_ON -= -mthreads
    QMAKE_LFLAGS_EXCEPTIONS_ON -= -mthreads
}
