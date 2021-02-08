#ifndef TORRENTPREVIEWABLEFILEPROPERTYEAGER_H
#define TORRENTPREVIEWABLEFILEPROPERTYEAGER_H

#include "orm/tiny/basemodel.hpp"

class TorrentPreviewableFilePropertyEager final :
        public Orm::Tiny::BaseModel<TorrentPreviewableFilePropertyEager>
{
public:
    friend class BaseModel;

    using BaseModel::BaseModel;

private:
    /*! The table associated with the model. */
    QString u_table {"torrent_previewable_file_properties"};

#ifdef PROJECT_TINYORM_TEST
    /*! The connection name for the model. */
    QString u_connection {"tinyorm_mysql_tests"};
#endif

    /*! Indicates if the model should be timestamped. */
    bool u_timestamps = false;
};

#endif // TORRENTPREVIEWABLEFILEPROPERTYEAGER_H
