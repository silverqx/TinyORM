#ifndef TORRENTPREVIEWABLEFILEPROPERTYEAGER_H
#define TORRENTPREVIEWABLEFILEPROPERTYEAGER_H

#include "orm/tiny/model.hpp"

using Orm::Tiny::Model;

class TorrentPreviewableFilePropertyEager final :
        public Model<TorrentPreviewableFilePropertyEager>
{
    friend Model;
    using Model::Model;

    /*! The table associated with the model. */
    QString u_table {"torrent_previewable_file_properties"};

    /*! Indicates if the model should be timestamped. */
    bool u_timestamps = false;
};

#endif // TORRENTPREVIEWABLEFILEPROPERTYEAGER_H
