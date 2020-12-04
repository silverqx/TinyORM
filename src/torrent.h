#ifndef TORRENTS_H
#define TORRENTS_H

#include "orm/tiny/basemodel.h"

#include "torrentpeer.h"
#include "torrentpreviewablefile.h"

/* This class serves as a showcase, so all possible features are defined / used. */

class Torrent final : public Orm::Tiny::BaseModel<Torrent, TorrentPreviewableFile, TorrentPeer>
{
public:
    friend class BaseModel;

    /*! The "type" of the primary key ID. */
    using KeyType = quint64;

    Torrent(const QVector<Orm::AttributeItem> &attributes = {});

    std::unique_ptr<
    Orm::Tiny::Relations::Relation<Torrent, TorrentPreviewableFile>>
    torrentFiles()
    {
        return hasMany<TorrentPreviewableFile>();
//        return hasMany<TorrentPreviewableFile>("torrent_id", "id");
    }

    std::unique_ptr<
    Orm::Tiny::Relations::Relation<Torrent, TorrentPeer>>
    torrentPeer()
    {
        return hasOne<TorrentPeer>();
//        return hasOne<TorrentPeer>("torrent_id", "id");
    }

private:
    void eagerVisitor(const QString &relation)
    {
        if (relation == "torrentFiles")
            eagerVisited<TorrentPreviewableFile>();
        else if (relation == "torrentPeer")
            eagerVisited<TorrentPeer>();
    }

    /*! The table associated with the model. */
    QString u_table {"torrents"};

    /*! Map of relation names to methods. */
    QHash<QString, std::any> u_relations {
        {"torrentFiles", &Torrent::torrentFiles},
        {"torrentPeer", &Torrent::torrentPeer},
    };

    /*! The relations to eager load on every query. */
    QVector<Orm::WithItem> u_with {
//        {"torrentFiles"},
        {"torrentPeer"},
    };

    /*! The connection name for the model. */
//    QString u_connection {"crystal"};
};

// TODO finish this, move to base class and test eg in qvector, qhash, etc silverqx
QDebug operator<<(QDebug debug, const Torrent &c);

#endif // TORRENTS_H
