#ifndef TORRENTS_H
#define TORRENTS_H

#include "orm/tiny/basemodel.hpp"

#include "models/forwards.hpp"

#include "models/torrentpeer.hpp"
#include "models/torrentpreviewablefile.hpp"

/* This class serves as a showcase, so all possible features are defined / used. */

class Torrent final : public Orm::Tiny::BaseModel<Torrent, TorrentPreviewableFile, TorrentPeer>
{
public:
    friend class BaseModel;

    /*! The "type" of the primary key ID. */
    using KeyType = quint64;

    using BaseModel::BaseModel;
//    explicit Torrent(const QVector<Orm::AttributeItem> &attributes = {});

    /*! Get the previewable files associated with the torrent. */
    std::unique_ptr<
    Orm::Tiny::Relations::Relation<Torrent, TorrentPreviewableFile>>
    torrentFiles()
    {
        return hasMany<TorrentPreviewableFile>();
//        return hasMany<TorrentPreviewableFile>("torrent_id", "id");
    }

    /*! Get the torrent peer associated with the torrent. */
    std::unique_ptr<
    Orm::Tiny::Relations::Relation<Torrent, TorrentPeer>>
    torrentPeer()
    {
        return hasOne<TorrentPeer>();
//        return hasOne<TorrentPeer>("torrent_id", "id");
    }

private:
    /*! The name of the "created at" column. */
    inline static const QString &CREATED_AT = QStringLiteral("created_at");
    /*! The name of the "updated at" column. */
    inline static const QString &UPDATED_AT = QStringLiteral("updated_at");

    /*! The visitor to obtain a type for Related template parameter. */
    void relationVisitor(const QString &relation)
    {
        if (relation == "torrentFiles")
            relationVisited<TorrentPreviewableFile>();
        else if (relation == "torrentPeer")
            relationVisited<TorrentPeer>();
    }

    /*! The table associated with the model. */
    QString u_table {"torrents"};

    /*! Indicates if the IDs are auto-incrementing. */
//    bool u_incrementing = true;
    /*! The primary key for the model. */
//    QString u_primaryKey {"id"};

    /*! Map of relation names to methods. */
    QHash<QString, std::any> u_relations {
        {"torrentFiles", &Torrent::torrentFiles},
        {"torrentPeer",  &Torrent::torrentPeer},
    };

    /*! The relations to eager load on every query. */
    QVector<Orm::WithItem> u_with {
//        {"torrentFiles"},
//        {"torrentPeer"},
//        {"torrentFiles.fileProperty"},
    };

    /*! The connection name for the model. */
#ifdef PROJECT_TINYORM_TEST
    QString u_connection {"tinyorm_mysql_tests"};
#else
    QString u_connection {"crystal"};
#endif

    /*! Indicates if the model should be timestamped. */
//    bool u_timestamps = true;
    /*! The storage format of the model's date columns. */
//    QString u_dateFormat {"yyyy-MM-dd HH:mm:ss"};
    /*! All of the relationships to be touched. */
//    QStringList u_touches {"relation_name"};
};

// TODO finish this, move to base class and test eg in qvector, qhash, etc silverqx
//QDebug operator<<(QDebug debug, const Torrent &c);

#endif // TORRENTS_H
