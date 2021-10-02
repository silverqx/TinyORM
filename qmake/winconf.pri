# WinApi
# ---

# All have to be defined because of checks at the beginning of the qt_windows.h
# Windows 10 1903 "19H1" - 0x0A000007
DEFINES += WINVER=_WIN32_WINNT_WIN10
DEFINES += NTDDI_VERSION=NTDDI_WIN10_19H1
DEFINES += _WIN32_WINNT=_WIN32_WINNT_WIN10
# Internet Explorer 11
DEFINES += _WIN32_IE=_WIN32_IE_IE110
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
