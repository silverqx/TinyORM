# WinApi
# ---

# Windows 10 1903 "19H1" - 0x0A000007
DEFINES += NTDDI_VERSION=0x0A000007
# Windows 10 - 0x0A00
DEFINES += _WIN32_WINNT=0x0A00
DEFINES += _WIN32_IE=0x0A00
DEFINES += UNICODE
DEFINES += _UNICODE
DEFINES += WIN32
DEFINES += _WIN32
DEFINES += WIN32_LEAN_AND_MEAN
DEFINES += NOMINMAX

# Compilers
# ---

win32-msvc* {
    # I don't use -MP flag, because using jom
    QMAKE_CXXFLAGS += -guard:cf -permissive- -Zc:ternary
    QMAKE_CXXFLAGS_DEBUG += -bigobj
    QMAKE_LFLAGS += /guard:cf
    QMAKE_LFLAGS_RELEASE += /OPT:REF /OPT:ICF=5
}
