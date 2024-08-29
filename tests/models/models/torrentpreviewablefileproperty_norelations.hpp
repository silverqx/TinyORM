#pragma once
#ifndef MODELS_TORRENTPREVIEWABLEFILEPROPERTY_NORELATIONS_HPP
#define MODELS_TORRENTPREVIEWABLEFILEPROPERTY_NORELATIONS_HPP

#include "orm/tiny/model.hpp"

namespace Models
{

using Orm::Tiny::Model;

class TorrentPreviewableFileProperty_NoRelations final : // NOLINT(bugprone-exception-escape)
        public Model<TorrentPreviewableFileProperty_NoRelations>
{
    friend Model;
    using Model::Model;

    /*! The table associated with the model. */
    QString u_table {"torrent_previewable_file_properties"};

    /*! Indicates whether the model should be timestamped. */
    bool u_timestamps = false;
};

} // namespace Models

#endif // MODELS_TORRENTPREVIEWABLEFILEPROPERTY_NORELATIONS_HPP
