#ifndef TORRENT_H
#define TORRENT_H

#include "orm/tiny/basemodel.hpp"

#include "models/forwards.hpp"

#include "models/tag.hpp"
#include "models/tagged.hpp"
#include "models/torrentpeer.hpp"
#include "models/torrentpreviewablefile.hpp"

// TODO now rewrite all with this 'using Xyz::Abc' style silverqx
using Orm::Tiny::BaseModel;
using Orm::Tiny::Relations::BelongsToMany;
using Orm::Tiny::Relations::Pivot;
using Orm::Tiny::Relations::Relation;
using Orm::WithItem;

/* This class serves as a showcase, so all possible features are defined / used. */

class Torrent final :
        public BaseModel<Torrent, TorrentPreviewableFile, TorrentPeer, Tag, Pivot>
//        public BaseModel<Torrent, TorrentPreviewableFile, TorrentPeer, Tag, Tagged>
{
    friend BaseModel;
    using BaseModel::BaseModel;

public:
    /*! The "type" of the primary key ID. */
    using KeyType = quint64;

//    explicit Torrent(const QVector<AttributeItem> &attributes = {});

    // TODO desirable, would be amazing to return derived relation type, like HasMany, I will have to solve this somehow silverqx
    /*! Get the previewable files associated with the torrent. */
    std::unique_ptr<Relation<Torrent, TorrentPreviewableFile>>
    torrentFiles()
    {
        return hasMany<TorrentPreviewableFile>();
//        return hasMany<TorrentPreviewableFile>("torrent_id", "id");
    }

    /*! Get the torrent peer associated with the torrent. */
    std::unique_ptr<Relation<Torrent, TorrentPeer>>
    torrentPeer()
    {
        return hasOne<TorrentPeer>();
//        return hasOne<TorrentPeer>("torrent_id", "id");
    }

    /*! Get tags that belong to the torrent. */
    std::unique_ptr<Relation<Torrent, Tag>>
    tags()
    {
        // Ownership of a unique_ptr()
//        auto relation = belongsToMany<Tag>();
//        dynamic_cast<BelongsToMany<Torrent, Tag/*, Pivot*/> &>(*relation)
//                .as("tagged")
//                .withPivot("active")
//                .withTimestamps();

        // Ownership of a unique_ptr()
        // Custom 'Tagged' pivot model ✨
        auto relation = belongsToMany<Tag, Tagged>();
        dynamic_cast<BelongsToMany<Torrent, Tag, Tagged> &>(*relation)
                .as("tagged")
                .withPivot("active")
                .withTimestamps(/*"created_at_custom", "updated_at_custom"*/);

        return relation;
//        return belongsToMany<Tag>("tag_torrent", "torrent_id", "tag_id", "id", "id",
//                                  "tags");
    }

private:
    /*! The name of the "created at" column. */
    inline static const QString CREATED_AT = QStringLiteral("created_at");
    /*! The name of the "updated at" column. */
    inline static const QString UPDATED_AT = QStringLiteral("updated_at");

    /*! The visitor to obtain a type for Related template parameter. */
    void relationVisitor(const QString &relation)
    {
        if (relation      == "torrentFiles")
            relationVisited<TorrentPreviewableFile>();
        else if (relation == "torrentPeer")
            relationVisited<TorrentPeer>();
        else if (relation == "tags")
            relationVisited<Tag>();
        else if (relation == "pivot") // Pivot
//            relationVisited<Tagged>();
            relationVisited<Pivot>();
    }

    /*! The table associated with the model. */
    QString u_table {"torrents"};

    /*! Indicates if the model's ID is auto-incrementing. */
//    bool u_incrementing = true;
    /*! The primary key associated with the table. */
//    QString u_primaryKey {"id"};

    /*! Map of relation names to methods. */
    QHash<QString, std::any> u_relations {
        {"torrentFiles", &Torrent::torrentFiles},
        {"torrentPeer",  &Torrent::torrentPeer},
        {"tags",         &Torrent::tags},
    };

    /*! The relations to eager load on every query. */
    QVector<WithItem> u_with {
//        {"torrentFiles"},
//        {"torrentPeer"},
//        {"torrentFiles.fileProperty"},
//        {"tags"},
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
//    QStringList u_touches {"tags"};
//    QStringList u_touches {"relation_name"};
};

// TODO finish this, move to base class and test eg in qvector, qhash, etc silverqx
//QDebug operator<<(QDebug debug, const Torrent &c);

#endif // TORRENT_H
