#ifndef TORRENTPEEREAGER_H
#define TORRENTPEEREAGER_H

#include "orm/tiny/basemodel.hpp"

#include "models/torrenteager.hpp"

class TorrentPeerEager final : public BaseModel<TorrentPeerEager, TorrentEager>
{
    friend BaseModel;
    using BaseModel::BaseModel;

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

#ifdef PROJECT_TINYORM_TEST
    /*! The connection name for the model. */
    QString u_connection {"tinyorm_mysql_tests"};
#endif
};

#endif // TORRENTPEEREAGER_H
