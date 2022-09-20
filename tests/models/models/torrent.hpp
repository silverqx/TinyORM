#pragma once
#ifndef MODELS_TORRENT_HPP
#define MODELS_TORRENT_HPP

#include "orm/db.hpp"
#include "orm/tiny/model.hpp"
#include "orm/tiny/relations/pivot.hpp"
//#include "orm/tiny/softdeletes.hpp"

#include "models/tag.hpp"
#include "models/tagged.hpp"
#include "models/torrentpeer.hpp"
#include "models/torrentpreviewablefile.hpp"
#include "models/user.hpp"

#ifdef PROJECT_TINYORM_PLAYGROUND
#  include "configuration.hpp"
#endif

namespace Models
{

//using Orm::AttributeItem;
using Orm::Constants::ID;
using Orm::Constants::NAME;
using Orm::Constants::SIZE;

//using Orm::Tiny::CastItem;
//using Orm::Tiny::CastType;
using Orm::Tiny::Model;
using Orm::Tiny::Relations::BelongsTo;
using Orm::Tiny::Relations::BelongsToMany;
using Orm::Tiny::Relations::HasOne;
using Orm::Tiny::Relations::HasMany;
using Orm::Tiny::Relations::Pivot;
//using Orm::Tiny::SoftDeletes;

#ifdef PROJECT_TINYORM_PLAYGROUND
using TinyPlay::Configuration;
#endif

/* This class serves as a showcase, so all possible features are defined / used. */

class Tag;
class TorrentPeer;
class TorrentPreviewableFile;
class User;

// NOLINTNEXTLINE(misc-no-recursion)
class Torrent final :
        public Model<Torrent, TorrentPreviewableFile, TorrentPeer, Tag, User, Pivot>
//        public Model<Torrent, TorrentPreviewableFile, TorrentPeer, Tag, User, Tagged>
//        public SoftDeletes<Torrent>
{
    friend Model;
    using Model::Model;

public:
    /*! Type used for the primary key ID. */
    using KeyType = quint64;

//    explicit Torrent(const QVector<AttributeItem> &attributes = {});

    /*! Get previewable files associated with the torrent. */
    std::unique_ptr<HasMany<Torrent, TorrentPreviewableFile>>
    torrentFiles()
    {
        return hasMany<TorrentPreviewableFile>();
//        return hasMany<TorrentPreviewableFile>("torrent_id", ID);
    }

    /*! Get a torrent peer associated with the torrent. */
    std::unique_ptr<HasOne<Torrent, TorrentPeer>>
    torrentPeer()
    {
        return hasOne<TorrentPeer>();
//        return hasOne<TorrentPeer>("torrent_id", ID);

        // Default Model example
//        auto relation = hasOne<TorrentPeer>();
//        relation->withDefault();
//        relation->withDefault({{"seeds", 0}, {"total_seeds", -1}});
        // This callback variant is not yet implemented 😟
//        relation->withDefault([](TorrentPeer &torrentPeer, const Torrent &/*torrent*/)
//        {
//            torrentPeer[NAME] = "default_peer_name";
//        });
//        return relation;
    }

    /*! Get tags that belong to the torrent. */
    std::unique_ptr<BelongsToMany<Torrent, Tag, Tagged>>
    tags()
    {
        // Ownership of a unique_ptr()
//        auto relation = belongsToMany<Tag>();
//        relation->as("tagged")
//                .withPivot("active")
//                .withTimestamps();

        // Ownership of a unique_ptr()
        // Custom 'Tagged' pivot model ✨
        auto relation = belongsToMany<Tag, Tagged>();
        relation->as("tagged")
                .withPivot("active")
                .withTimestamps(/*"created_at_custom", "updated_at_custom"*/);

        return relation;
//        return belongsToMany<Tag>("tag_torrent", "torrent_id", "tag_id", ID, ID,
//                                  "tags");
    }

    /*! Get a user that owns the torrent. */
    std::unique_ptr<BelongsTo<Torrent, User>>
    user()
    {
        return belongsTo<User>();
    }

private:
    /*! The name of the "created at" column. */
    inline static const QString &CREATED_AT = Orm::CREATED_AT; // NOLINT(cppcoreguidelines-interfaces-global-init)
    /*! The name of the "updated at" column. */
    inline static const QString &UPDATED_AT = Orm::UPDATED_AT; // NOLINT(cppcoreguidelines-interfaces-global-init)

    /*! The table associated with the model. */
    QString u_table {"torrents"};

    /*! Indicates if the model's ID is auto-incrementing. */
//    bool u_incrementing = true;
    /*! The primary key associated with the table. */
//    QString u_primaryKey {ID};

    /*! Map of relation names to methods. */
    QHash<QString, RelationVisitor> u_relations {
        {"torrentFiles", [](auto &v) { v(&Torrent::torrentFiles); }},
        {"torrentPeer",  [](auto &v) { v(&Torrent::torrentPeer); }},
        {"tags",         [](auto &v) { v(&Torrent::tags); }},
        {"user",         [](auto &v) { v(&Torrent::user); }},
    };

    /*! The relations to eager load on every query. */
    QVector<QString> u_with {
//        "torrentFiles",
//        "torrentPeer",
//        "torrentFiles.fileProperty",
//        "tags",
//        "user",
    };

#ifdef PROJECT_TINYORM_PLAYGROUND
    // I leave the initializer here to be clearly visible
    /*! The connection name for the model. */
    QString u_connection {Orm::DB::getDefaultConnection() == Mysql
                          ? Mysql_Alt
                          : Orm::DB::getDefaultConnection()};
#endif

    /*! The connection name for the model. */
//    QString u_connection {"sqlite"};

    /*! The model's default values for attributes. */
//    inline static const QVector<AttributeItem> u_attributes {
//        {SIZE,       0},
//        {"progress", 0},
//        {"added_on", QDateTime::fromString("2021-04-01 15:10:10", Qt::ISODate)},
//    };

    /*! The attributes that are mass assignable. */
    inline static const QStringList u_fillable { // NOLINT(cppcoreguidelines-interfaces-global-init)
        ID,
        NAME,
        SIZE,
        "progress",
        "added_on",
        "hash",
        "note",
        UPDATED_AT,
    };

    /*! The attributes that aren't mass assignable. */
//    inline static QStringList u_guarded {
//        password_,
//    };

    /*! Indicates whether the model should be timestamped. */
//    bool u_timestamps = true;

    /*! The storage format of the model's date columns. */
//    inline static QString u_dateFormat {"yyyy-MM-dd HH:mm:ss"};

    /*! The attributes that should be mutated to dates. */
    inline static const QStringList u_dates {"added_on", "added_on_alt"};

    /*! All of the relationships to be touched. */
//    QStringList u_touches {"tags"};
//    QStringList u_touches {"relation_name"};

    /*! The attributes that should be cast. */
//    std::unordered_map<QString, CastItem> u_casts {
//        {NAME,       CastType::QString},
//        {"progress", CastType::UShort},
//        // Showcase only, the Torrent model doesn't have the decimal column
//        {"decimal",  CastType::Decimal},
//        {"decimal",  {CastType::Decimal, 2}},
//    };
};

} // namespace Models

// TODO finish this, move to base class and test eg in qvector, qhash, etc silverqx
//QDebug operator<<(QDebug debug, const Models::Torrent &c);

#endif // MODELS_TORRENT_HPP
