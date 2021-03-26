#ifndef TORRENTPEER_H
#define TORRENTPEER_H

#include "orm/tiny/basemodel.hpp"

#include "models/torrent.hpp"

class TorrentPeer final : public BaseModel<TorrentPeer, Torrent>
{
    friend BaseModel;
    using BaseModel::BaseModel;

public:
    /*! Get the torrent that owns the torrent peer. */
    std::unique_ptr<Relation<TorrentPeer, Torrent>>
    torrent()
    {
        return belongsTo<Torrent>();
    }

private:
    /*! The visitor to obtain a type for Related template parameter. */
    void relationVisitor(const QString &relation)
    {
        if (relation == "torrent")
            relationVisited<Torrent>();
    }

    /*! The table associated with the model. */
    QString u_table {"torrent_peers"};

    /*! Map of relation names to methods. */
    QHash<QString, std::any> u_relations {
        {"torrent", &TorrentPeer::torrent},
    };

    /*! The relations to eager load on every query. */
    QVector<WithItem> u_with {
//        {"torrent"},
    };
};

#endif // TORRENTPEER_H
