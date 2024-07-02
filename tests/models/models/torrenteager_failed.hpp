#pragma once
#ifndef MODELS_TORRENTEAGER_FAILED_HPP
#define MODELS_TORRENTEAGER_FAILED_HPP

#include "orm/tiny/model.hpp"

#include "models/torrentpreviewablefileeager.hpp"

namespace Models
{

using Orm::Tiny::Model;

class TorrentEager_Failed final : // NOLINT(bugprone-exception-escape, misc-no-recursion)
        public Model<TorrentEager_Failed, TorrentPreviewableFileEager>
{
    friend Model;
    using Model::Model;

public:
    /*! Get previewable files associated with the torrent. */
    std::unique_ptr<HasMany<TorrentEager_Failed, TorrentPreviewableFileEager>>
    torrentFiles()
    {
        return hasMany<TorrentPreviewableFileEager>("torrent_id");
    }

private:
    /*! The table associated with the model. */
    QString u_table {"torrents"};

    /*! Map of relation names to methods. */
    QHash<QString, RelationVisitor> u_relations {
        {"torrentFiles", [](auto &v) { v(&TorrentEager_Failed::torrentFiles); }},
    };

    /*! The relations to eager load on every query. */
    QList<QString> u_with {
        "torrentFiles-NON_EXISTENT",
    };

    /*! The attributes that should be mutated to dates. */
    inline static const QStringList u_dates {"added_on"};
};

} // namespace Models

#endif // MODELS_TORRENTEAGER_FAILED_HPP
