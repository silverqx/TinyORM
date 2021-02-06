# range-v3
INCLUDEPATH += $$quote(E:/c/qMedia/vcpkg/installed/x64-windows/include)

INCLUDEPATH += $$PWD

SOURCES += \
    $$PWD/models/filepropertyproperty.cpp \
    $$PWD/models/setting.cpp \
    $$PWD/models/torrent.cpp \
    $$PWD/models/torrentpeer.cpp \
    $$PWD/models/torrentpreviewablefile.cpp \
    $$PWD/models/torrentpreviewablefileproperty.cpp \

HEADERS += \
    $$PWD/models/filepropertyproperty.hpp \
    $$PWD/models/forwards.hpp \
    $$PWD/models/forwardseager.hpp \
    $$PWD/models/setting.hpp \
    $$PWD/models/torrent.hpp \
    $$PWD/models/torrenteager.hpp \
    $$PWD/models/torrenteager_failed.hpp \
    $$PWD/models/torrentpeer.hpp \
    $$PWD/models/torrentpeereager.hpp \
    $$PWD/models/torrentpreviewablefile.hpp \
    $$PWD/models/torrentpreviewablefileeager.hpp \
    $$PWD/models/torrentpreviewablefileproperty.hpp \
    $$PWD/models/torrentpreviewablefilepropertyeager.hpp \
