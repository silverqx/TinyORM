#pragma once
#ifndef MODELS_TORRENTPREVIEWABLEFILE_NORELATIONS_HPP
#define MODELS_TORRENTPREVIEWABLEFILE_NORELATIONS_HPP

#include "orm/tiny/model.hpp"

namespace Models
{

using Orm::Tiny::Model;

class TorrentPreviewableFile_NoRelations final : // NOLINT(bugprone-exception-escape)
        public Model<TorrentPreviewableFile_NoRelations>
{
    friend Model;
    using Model::Model;

    /*! The table associated with the model. */
    QString u_table {u"torrent_previewable_files"_s};
};

} // namespace Models

#endif // MODELS_TORRENTPREVIEWABLEFILE_NORELATIONS_HPP
