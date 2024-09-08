#pragma once
#ifndef MODELS_TORRENTPEEREAGER_HPP
#define MODELS_TORRENTPEEREAGER_HPP

#include "orm/tiny/model.hpp"

namespace Models
{

using Orm::Tiny::Model;

class Torrent_NoRelations;

class TorrentPeerEager final : public Model<TorrentPeerEager, Torrent_NoRelations> // NOLINT(bugprone-exception-escape, misc-no-recursion)
{
    friend Model;
    using Model::Model;

public:
    /*! Get a torrent that owns the torrent peer. */
    std::unique_ptr<BelongsTo<TorrentPeerEager, Torrent_NoRelations>>
    torrent()
    {
        return belongsTo<Torrent_NoRelations>({}, {}, QString::fromUtf8(__func__)); // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    }

private:
    /*! The table associated with the model. */
    QString u_table {u"torrent_peers"_s};

    /*! Map of relation names to methods. */
    QHash<QString, RelationVisitor> u_relations {
        {u"torrent"_s, [](auto &v) { v(&TorrentPeerEager::torrent); }},
    };

    /*! The relations to eager load on every query. */
    QList<QString> u_with {
        u"torrent"_s,
    };
};

} // namespace Models

#endif // MODELS_TORRENTPEEREAGER_HPP
