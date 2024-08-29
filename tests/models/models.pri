INCLUDEPATH *= $$PWD

# Model Include lists (used to avoid duplicate #include-s) 😮
headersList = \
    $$PWD/models/album_includeslist.hpp \
    $$PWD/models/torrent_includeslist.hpp \
    $$PWD/models/torrent_returnrelation_includeslist.hpp \
    $$PWD/models/torrenteager_includeslist.hpp \
    $$PWD/models/torrentpeereager_includeslist.hpp \
    $$PWD/models/torrentpreviewablefileeager_withdefault_includeslist.hpp \

# Individual Model classes
headersList += \
    $$PWD/models/album.hpp \
    $$PWD/models/albumimage.hpp \
    $$PWD/models/datetime.hpp \
    $$PWD/models/datetime_serializeoverride.hpp \
    $$PWD/models/filepropertyproperty.hpp \
    $$PWD/models/massassignmentmodels.hpp \
    $$PWD/models/phone.hpp \
    $$PWD/models/role.hpp \
    $$PWD/models/roleuser.hpp \
    $$PWD/models/roleuser_appends.hpp \
    $$PWD/models/setting.hpp \
    $$PWD/models/tag.hpp \
    $$PWD/models/tag_basicpivot_norelations.hpp \
    $$PWD/models/tag_custompivot_norelations.hpp \
    $$PWD/models/tagged.hpp \
    $$PWD/models/tagproperty.hpp \
    $$PWD/models/torrent.hpp \
    $$PWD/models/torrent_norelations.hpp \
    $$PWD/models/torrent_returnrelation.hpp \
    $$PWD/models/torrent_with_qdatetime.hpp \
    $$PWD/models/torrent_without_qdatetime.hpp \
    $$PWD/models/torrenteager.hpp \
    $$PWD/models/torrenteager_failed.hpp \
    $$PWD/models/torrentpeer.hpp \
    $$PWD/models/torrentpeer_norelations.hpp \
    $$PWD/models/torrentpeereager.hpp \
    $$PWD/models/torrentpreviewablefile.hpp \
    $$PWD/models/torrentpreviewablefile_norelations.hpp \
    $$PWD/models/torrentpreviewablefileeager.hpp \
    $$PWD/models/torrentpreviewablefileeager_withdefault.hpp \
    $$PWD/models/torrentpreviewablefileproperty.hpp \
    $$PWD/models/torrentpreviewablefileproperty_norelations.hpp \
    $$PWD/models/torrentstate.hpp \
    $$PWD/models/type.hpp \
    $$PWD/models/user.hpp \
    $$PWD/models/user_norelations.hpp \

HEADERS += $$sorted(headersList)

unset(headersList)
