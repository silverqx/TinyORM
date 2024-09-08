#pragma once
#ifndef MODELS_TORRENTPEER_HPP
#define MODELS_TORRENTPEER_HPP

#include "orm/tiny/model.hpp"

namespace Models
{

using Orm::Tiny::Model;

class Torrent;

class TorrentPeer final : public Model<TorrentPeer, Torrent> // NOLINT(bugprone-exception-escape, misc-no-recursion)
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
    /*! Map of relation names to methods. */
    QHash<QString, RelationVisitor> u_relations {
        {u"torrent"_s, [](auto &v) { v(&TorrentPeer::torrent); }},
    };
};

} // namespace Models

#endif // MODELS_TORRENTPEER_HPP
