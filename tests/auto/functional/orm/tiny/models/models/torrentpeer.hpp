#ifndef TORRENTPEERS_H
#define TORRENTPEERS_H

#include "orm/tiny/basemodel.hpp"

#include "models/torrent.hpp"

class TorrentPeer final : public Orm::Tiny::BaseModel<TorrentPeer, Torrent>
{
public:
    friend class BaseModel;

    explicit TorrentPeer(const QVector<Orm::AttributeItem> &attributes = {});

    /*! Get the torrent that owns the torrent peer. */
    std::unique_ptr<
    Orm::Tiny::Relations::Relation<TorrentPeer, Torrent>>
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
    QVector<Orm::WithItem> u_with {
//        {"torrent"},
    };

#ifdef PROJECT_TINYORM_TEST
    /*! The connection name for the model. */
    QString u_connection {"tinyorm_mysql_tests"};
#endif
};

#endif // TORRENTPEERS_H
