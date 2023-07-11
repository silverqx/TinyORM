#pragma once
#ifndef MODELS_TORRENTEAGER_HPP
#define MODELS_TORRENTEAGER_HPP

#include "orm/tiny/model.hpp"

#include "models/torrentpeereager_norelations.hpp"
#include "models/torrentpreviewablefileeager.hpp"

namespace Models
{

using Orm::Constants::HASH_;
using Orm::Constants::NAME;
using Orm::Constants::NOTE;
using Orm::Constants::Progress;
using Orm::Constants::SIZE_;

using Orm::Tiny::AttributeItem;
using Orm::Tiny::Model;
using Orm::Tiny::Relations::HasOne;
using Orm::Tiny::Relations::HasMany;

// NOLINTNEXTLINE(bugprone-exception-escape)
class TorrentEager final :
        public Model<TorrentEager, TorrentPreviewableFileEager,
                     TorrentPeerEager_NoRelations>
{
    friend Model;
    using Model::Model;

public:
    /*! Get previewable files associated with the torrent. */
    std::unique_ptr<HasMany<TorrentEager, TorrentPreviewableFileEager>>
    torrentFiles()
    {
        return hasMany<TorrentPreviewableFileEager>("torrent_id");
    }

    /*! Get a torrent peer associated with the torrent. */
    std::unique_ptr<HasOne<TorrentEager, TorrentPeerEager_NoRelations>>
    torrentPeer()
    {
        return hasOne<TorrentPeerEager_NoRelations>("torrent_id");
    }

private:
    /*! The table associated with the model. */
    QString u_table {"torrents"};

    /*! Map of relation names to methods. */
    QHash<QString, RelationVisitor> u_relations {
        {"torrentFiles", [](auto &v) { v(&TorrentEager::torrentFiles); }},
        {"torrentPeer",  [](auto &v) { v(&TorrentEager::torrentPeer); }},
    };

    /*! The relations to eager load on every query. */
    QVector<QString> u_with {
        "torrentFiles.fileProperty",
        "torrentPeer",
    };

    /*! The model's default values for attributes. */
    inline static const QVector<AttributeItem> u_attributes { // NOLINT(cppcoreguidelines-interfaces-global-init)
        {SIZE_,      0},
        {Progress,   0},
        {"added_on", QDateTime({2021, 4, 1}, {15, 10, 10}, Qt::UTC)},
    };

    /*! The attributes that are mass assignable. */
    inline static const QStringList u_fillable { // NOLINT(cppcoreguidelines-interfaces-global-init)
        NAME,
        SIZE_,
        Progress,
        "added_on",
        HASH_,
        NOTE,
    };

    /*! The attributes that should be mutated to dates. */
    inline static const QStringList u_dates {"added_on"};
};

} // namespace Models

#endif // MODELS_TORRENTEAGER_HPP
