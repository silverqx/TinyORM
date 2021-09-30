#pragma once
#ifndef TORRENTEAGER_HPP
#define TORRENTEAGER_HPP

#include "orm/tiny/model.hpp"

#include "models/torrentpeereager_norelations.hpp"
#include "models/torrentpreviewablefileeager.hpp"

using namespace Orm::Constants;

using Orm::AttributeItem;
using Orm::Tiny::Model;
using Orm::Tiny::Relations::HasOne;
using Orm::Tiny::Relations::HasMany;

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
    inline static const QVector<AttributeItem> u_attributes {
        {"size",     0},
        {"progress", 0},
        {"added_on", QDateTime::fromString("2021-04-01 15:10:10", Qt::ISODate)},
    };

    /*! The attributes that are mass assignable. */
    inline static QStringList u_fillable {
        NAME,
        "size",
        "progress",
        "added_on",
        "hash",
        "note",
    };

    /*! The attributes that should be mutated to dates. @deprecated */
    inline static QStringList u_dates {"added_on"};
};

#endif // TORRENTEAGER_HPP
