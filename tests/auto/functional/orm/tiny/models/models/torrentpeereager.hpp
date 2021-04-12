#ifndef TORRENTPEEREAGER_H
#define TORRENTPEEREAGER_H

#include "orm/tiny/model.hpp"

#include "models/torrenteager.hpp"

class TorrentPeerEager final : public Model<TorrentPeerEager, TorrentEager>
{
    friend Model;
    using Model::Model;

public:
    /*! Get the torrent that owns the torrent peer. */
    std::unique_ptr<Relation<TorrentPeerEager, TorrentEager>>
    torrent()
    {
        return belongsTo<TorrentEager>({}, {}, __func__);
    }

private:
    /*! The visitor to obtain a type for Related template parameter. */
    void relationVisitor(const QString &relation)
    {
        if (relation == "torrent")
            relationVisited<TorrentEager>();
    }

    /*! The table associated with the model. */
    QString u_table {"torrent_peers"};

    /*! Map of relation names to methods. */
    QHash<QString, std::any> u_relations {
        {"torrent", &TorrentPeerEager::torrent},
    };

    /*! The relations to eager load on every query. */
    QVector<WithItem> u_with {
        {"torrent"},
    };
};

#endif // TORRENTPEEREAGER_H
