#pragma once
#ifndef MODELS_TORRENTPEER_HPP
#define MODELS_TORRENTPEER_HPP

#include "orm/tiny/model.hpp"

#include "models/torrent.hpp"

namespace Models
{

using Orm::Tiny::Model;

class Torrent;

// NOLINTNEXTLINE(misc-no-recursion, bugprone-exception-escape)
class TorrentPeer final : public Model<TorrentPeer, Torrent>
{
    friend Model;
    using Model::Model;

public:
    /*! Get a torrent that owns the torrent peer. */
    std::unique_ptr<BelongsTo<TorrentPeer, Torrent>>
    torrent()
    {
        return belongsTo<Torrent>();
    }

private:
    /*! The table associated with the model. */
    QString u_table {"torrent_peers"};

    /*! Map of relation names to methods. */
    QHash<QString, RelationVisitor> u_relations {
        {"torrent", [](auto &v) { v(&TorrentPeer::torrent); }},
    };

    /*! The relations to eager load on every query. */
    QVector<QString> u_with { // NOLINT(readability-redundant-member-init)
//        "torrent",
    };
};

} // namespace Models

#endif // MODELS_TORRENTPEER_HPP
