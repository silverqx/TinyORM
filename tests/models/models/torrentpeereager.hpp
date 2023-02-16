#pragma once
#ifndef MODELS_TORRENTPEEREAGER_HPP
#define MODELS_TORRENTPEEREAGER_HPP

#include "orm/tiny/model.hpp"

#include "models/torrenteager.hpp"

namespace Models
{

using Orm::Tiny::Relations::BelongsTo;

// NOLINTNEXTLINE(bugprone-exception-escape)
class TorrentPeerEager final : public Model<TorrentPeerEager, TorrentEager>
{
    friend Model;
    using Model::Model;

public:
    /*! Get a torrent that owns the torrent peer. */
    std::unique_ptr<BelongsTo<TorrentPeerEager, TorrentEager>>
    torrent()
    {
        return belongsTo<TorrentEager>({}, {}, QString::fromUtf8(__func__)); // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    }

private:
    /*! The table associated with the model. */
    QString u_table {"torrent_peers"};

    /*! Map of relation names to methods. */
    QHash<QString, RelationVisitor> u_relations {
        {"torrent", [](auto &v) { v(&TorrentPeerEager::torrent); }},
    };

    /*! The relations to eager load on every query. */
    QVector<QString> u_with {
        "torrent",
    };
};

} // namespace Models

#endif // MODELS_TORRENTPEEREAGER_HPP
