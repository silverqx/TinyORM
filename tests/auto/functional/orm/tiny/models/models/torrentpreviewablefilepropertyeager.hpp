#ifndef TORRENTPREVIEWABLEFILEPROPERTYEAGER_H
#define TORRENTPREVIEWABLEFILEPROPERTYEAGER_H

#include "orm/tiny/basemodel.hpp"

using Orm::Tiny::BaseModel;

class TorrentPreviewableFilePropertyEager final :
        public BaseModel<TorrentPreviewableFilePropertyEager>
{
    friend BaseModel;
    using BaseModel::BaseModel;

    /*! The table associated with the model. */
    QString u_table {"torrent_previewable_file_properties"};

    /*! Indicates if the model should be timestamped. */
    bool u_timestamps = false;
};

#endif // TORRENTPREVIEWABLEFILEPROPERTYEAGER_H
