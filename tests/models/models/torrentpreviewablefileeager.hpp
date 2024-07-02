#pragma once
#ifndef MODELS_TORRENTPREVIEWABLEFILEEAGER_HPP
#define MODELS_TORRENTPREVIEWABLEFILEEAGER_HPP

#include "orm/tiny/model.hpp"

#include "models/torrentpreviewablefilepropertyeager.hpp"

namespace Models
{

using Orm::Constants::NOTE;
using Orm::Constants::Progress;
using Orm::Constants::SIZE_;

using Orm::Tiny::Model;

class TorrentPreviewableFileEager final : // NOLINT(bugprone-exception-escape, misc-no-recursion)
        public Model<TorrentPreviewableFileEager, TorrentPreviewableFilePropertyEager>
{
    friend Model;
    using Model::Model;

public:
    /*! Get a file property associated with the previewable file. */
    std::unique_ptr<HasOne<TorrentPreviewableFileEager,
                           TorrentPreviewableFilePropertyEager>>
    fileProperty()
    {
        return hasOne<TorrentPreviewableFilePropertyEager>("previewable_file_id");
    }

private:
    /*! The table associated with the model. */
    QString u_table {"torrent_previewable_files"};

    /*! Map of relation names to methods. */
    QHash<QString, RelationVisitor> u_relations {
        {"fileProperty", [](auto &v) { v(&TorrentPreviewableFileEager::fileProperty); }},
    };

    /*! The relations to eager load on every query. */
    QList<QString> u_with {
        "fileProperty",
    };

    /*! The attributes that are mass assignable. */
    inline static QStringList u_fillable { // NOLINT(cppcoreguidelines-interfaces-global-init)
        "file_index",
        "filepath",
        SIZE_,
        Progress,
        NOTE,
    };
};

} // namespace Models

#endif // MODELS_TORRENTPREVIEWABLEFILEEAGER_HPP
