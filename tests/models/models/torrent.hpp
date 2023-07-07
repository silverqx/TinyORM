#pragma once
#ifndef MODELS_TORRENT_HPP
#define MODELS_TORRENT_HPP

#ifdef PROJECT_TINYORM_PLAYGROUND
#  include "orm/db.hpp"
#endif
#include "orm/tiny/relations/pivot.hpp"
//#include "orm/tiny/softdeletes.hpp"

#include "models/tag.hpp"
#include "models/tagged.hpp"
#include "models/torrentpeer.hpp"
#include "models/torrentpreviewablefile.hpp"
#include "models/torrentstate.hpp"
#include "models/user.hpp"

#ifdef PROJECT_TINYORM_PLAYGROUND
#  include "configuration.hpp"
#endif

namespace Models
{

//using Orm::AttributeItem;
using Orm::Constants::HASH_;
using Orm::Constants::ID;
using Orm::Constants::NAME;
using Orm::Constants::NOTE;
using Orm::Constants::SIZE_;

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
class TorrentState;
class User;

// NOLINTNEXTLINE(misc-no-recursion, bugprone-exception-escape)
class Torrent final :
        public Model<Torrent, TorrentPreviewableFile, TorrentPeer, Tag, User,
                     TorrentState, Pivot>
//        public Model<Torrent, TorrentPreviewableFile, TorrentPeer, Tag, User,
//                     TorrentState, Tagged>
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
//                 .withPivot("active")
//                 .withTimestamps();

        // Custom 'Tagged' pivot model ✨
        // Ownership of a unique_ptr()
        auto relation = belongsToMany<Tag, Tagged>();
        relation->as("tagged")
                 .withPivot("active")
                 .withTimestamps(/*"created_at_custom", "updated_at_custom"*/);

        return relation;

        // Basic pivot model
//        return belongsToMany<Tag>("tag_torrent", "torrent_id", "tag_id", ID, ID,
//                                  "tags");
    }

    /*! Get a user that owns the torrent. */
    std::unique_ptr<BelongsTo<Torrent, User>>
    user()
    {
        return belongsTo<User>();
    }

    /*! Get torrent states that belong to the torrent. */
    std::unique_ptr<BelongsToMany<Torrent, TorrentState>>
    torrentStates()
    {
        // Ownership of a unique_ptr()
        auto relation = belongsToMany<TorrentState>("state_torrent", {}, "state_id");

        relation->withPivot("active");

        return relation;
    }

private:
    /*! The name of the "created at" column. */
    inline static const QString &CREATED_AT() noexcept { return Orm::CREATED_AT; }
    /*! The name of the "updated at" column. */
    inline static const QString &UPDATED_AT() noexcept { return Orm::UPDATED_AT; }

    /*! The table associated with the model. */
    QString u_table {"torrents"};

    /*! Indicates if the model's ID is auto-incrementing. */
//    bool u_incrementing = true;
    /*! The primary key associated with the table. */
//    QString u_primaryKey {ID};

    /*! Map of relation names to methods. */
    QHash<QString, RelationVisitor> u_relations {
        {"torrentFiles",  [](auto &v) { v(&Torrent::torrentFiles); }},
        {"torrentPeer",   [](auto &v) { v(&Torrent::torrentPeer); }},
        {"tags",          [](auto &v) { v(&Torrent::tags); }},
        {"user",          [](auto &v) { v(&Torrent::user); }},
        {"torrentStates", [](auto &v) { v(&Torrent::torrentStates); }},
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
//        {SIZE_,      0},
//        {"progress", 0},
//        {"added_on", QDateTime({2021, 4, 1}, {15, 10, 10}, Qt::UTC)},
//    };

    /*! The attributes that are mass assignable. */
    inline static const QStringList u_fillable { // NOLINT(cppcoreguidelines-interfaces-global-init)
        ID,
        NAME,
        SIZE_,
        "progress",
        "added_on",
        HASH_,
        NOTE,
        UPDATED_AT(),
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
//    inline static std::unordered_map<QString, CastItem> u_casts {
//        {NAME,       CastType::QString},
//        {"progress", CastType::UShort},
//        // Showcase only, the Torrent model doesn't have the decimal column
//        {"decimal",  CastType::Decimal},
//        {"decimal",  {CastType::Decimal, 2}},
//    };

    /*! Indicates whether attributes are snake_cased during serialization. */
//    inline static const bool u_snakeAttributes = false;

    /*! The attributes that should be visible during serialization. */
//    inline static std::set<QString> u_visible {
//        ID, "user_id", NAME, SIZE_, "progress", NOTE, CREATED_AT(), UPDATED_AT(),
//    };
    /*! The attributes that should be hidden during serialization. */
//    inline static std::set<QString> u_hidden {
//        "added_on", HASH_,
//    };
};

} // namespace Models

// TODO finish this, move to base class and test eg in qvector, qhash, etc silverqx
//QDebug operator<<(QDebug debug, const Models::Torrent &c);

#endif // MODELS_TORRENT_HPP
