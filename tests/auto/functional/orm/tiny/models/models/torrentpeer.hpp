#ifndef TORRENTPEER_H
#define TORRENTPEER_H

#include "orm/tiny/model.hpp"

#include "models/torrent.hpp"

class TorrentPeer final : public Model<TorrentPeer, Torrent>
{
    friend Model;
    using Model::Model;

public:
    /*! Get the torrent that owns the torrent peer. */
    std::unique_ptr<Relation<TorrentPeer, Torrent>>
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
    QVector<WithItem> u_with {
//        {"torrent"},
    };
};

#endif // TORRENTPEER_H
