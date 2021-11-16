#pragma once
#ifndef MODELS_TORRENTPREVIEWABLEFILEPROPERTYEAGER_HPP
#define MODELS_TORRENTPREVIEWABLEFILEPROPERTYEAGER_HPP

#include "orm/tiny/model.hpp"

namespace Models
{

using Orm::Tiny::Model;

class TorrentPreviewableFilePropertyEager final :
        public Model<TorrentPreviewableFilePropertyEager>
{
    friend Model;
    using Model::Model;

    /*! The table associated with the model. */
    QString u_table {"torrent_previewable_file_properties"};

    /*! Indicates whether the model should be timestamped. */
    bool u_timestamps = false;
};

} // namespace Models

#endif // MODELS_TORRENTPREVIEWABLEFILEPROPERTYEAGER_HPP
