#pragma once
#ifndef MODELS_TORRENTPREVIEWABLEFILEEAGER_HPP
#define MODELS_TORRENTPREVIEWABLEFILEEAGER_HPP

#include "orm/tiny/model.hpp"

namespace Models
{

using Orm::Constants::NOTE;
using Orm::Constants::Progress;
using Orm::Constants::SIZE_;

using Orm::Tiny::Model;

class TorrentPreviewableFileProperty_NoRelations;

class TorrentPreviewableFileEager final : // NOLINT(bugprone-exception-escape, misc-no-recursion)
        public Model<TorrentPreviewableFileEager,
                     TorrentPreviewableFileProperty_NoRelations>
{
    friend Model;
    using Model::Model;

public:
    /*! Get a file property associated with the previewable file. */
    std::unique_ptr<HasOne<TorrentPreviewableFileEager,
                           TorrentPreviewableFileProperty_NoRelations>>
    fileProperty()
    {
        return hasOne<TorrentPreviewableFileProperty_NoRelations>(
                    u"previewable_file_id"_s);
    }

private:
    /*! The table associated with the model. */
    QString u_table {u"torrent_previewable_files"_s};

    /*! Map of relation names to methods. */
    QHash<QString, RelationVisitor> u_relations {
        {u"fileProperty"_s, [](auto &v) { v(&TorrentPreviewableFileEager::fileProperty); }},
    };

    /*! The relations to eager load on every query. */
    QList<QString> u_with {
        u"fileProperty"_s,
    };

    /*! The attributes that are mass assignable. */
    inline static QStringList u_fillable { // NOLINT(cppcoreguidelines-interfaces-global-init)
        u"file_index"_s,
        u"filepath"_s,
        SIZE_,
        Progress,
        NOTE,
    };
};

} // namespace Models

#endif // MODELS_TORRENTPREVIEWABLEFILEEAGER_HPP
