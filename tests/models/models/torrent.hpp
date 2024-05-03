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
using Orm::Constants::PARENTH_ONE;
using Orm::Constants::Progress;
using Orm::Constants::SIZE_;
using Orm::Constants::SPACE_IN;

using Orm::Tiny::Model;
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

class Torrent final : // NOLINT(bugprone-exception-escape, misc-no-recursion)
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

protected:
    /*! Accessor for torrent name with progress. Example: torrent 1 (98). */
    Attribute nameProgress() const noexcept
    {
        return Attribute::make(/* get */ [this]() -> QVariant
        {
            return SPACE_IN.arg(getAttribute<QString>(NAME),
                                PARENTH_ONE.arg(getAttribute<quint16>(Progress)));
        }).shouldCache();
    }

    /*! Accessor for torrent name with size. Example: torrent 1 (98). */
    Attribute nameSize() const noexcept // NOLINT(readability-convert-member-functions-to-static)
    {
        return Attribute::make(
               /* get */ [](const ModelAttributes &attributes) -> QVariant
        {
            return SPACE_IN.arg(attributes.at<QString>(NAME),
                                PARENTH_ONE.arg(attributes.at<quint64>(SIZE_)));
        });
    }

private:
    /*! The name of the "created at" column. */
    static const QString &CREATED_AT() noexcept { return Orm::CREATED_AT; }
    /*! The name of the "updated at" column. */
    static const QString &UPDATED_AT() noexcept { return Orm::UPDATED_AT; }

    /* Serialization */
    /*! Prepare a date for vector, map, or JSON serialization. */
//    inline static QString serializeDate(const QDate date)
//    {
//        return date.toString("dd.MM.yyyy");
//    }

    /*! Prepare a datetime for vector, map, or JSON serialization. */
//    inline static QString serializeDateTime(const QDateTime &datetime)
//    {
//        return datetime.toUTC().toString("dd.MM.yyyy HH:mm:ss.z t");
//    }

    /* Data members */
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
    QVector<QString> u_with { // NOLINT(readability-redundant-member-init)
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
//        {Progress, 0},
//        {"added_on", QDateTime({2021, 4, 1}, {15, 10, 10}, Qt::UTC)},
//    };

    /*! The attributes that are mass assignable. */
    inline static const QStringList u_fillable { // NOLINT(cppcoreguidelines-interfaces-global-init)
        ID,
        NAME,
        SIZE_,
        Progress,
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
    /*! The storage format of the model's time columns. */
//    inline static QString u_timeFormat {"HH:mm:ss"};

    /*! The attributes that should be mutated to dates. */
    inline static const QStringList u_dates {"added_on", "added_on_alt"};

    /*! All of the relationships to be touched. */
//    QStringList u_touches {"tags"};
//    QStringList u_touches {"relation_name"};

    /*! The attributes that should be cast. */
//    inline static std::unordered_map<QString, CastItem> u_casts {
//        {NAME,      CastType::QString},
//        {Progress,  CastType::UShort},
//        // Showcase only, the Torrent model doesn't have the decimal column
//        {"decimal", CastType::Decimal},
//        {"decimal", {CastType::Decimal, 2}},
//    };

    /* Serialization */
    /*! Indicates whether attributes are snake_cased during serialization. */
//    inline static const bool u_snakeAttributes = false;

    /* HidesAttributes */
    /*! The attributes that should be visible during serialization. */
//    inline static std::set<QString> u_visible {
//        ID, "user_id", NAME, SIZE_, Progress, NOTE, CREATED_AT(), UPDATED_AT(),
//    };
    /*! The attributes that should be hidden during serialization. */
//    inline static std::set<QString> u_hidden {
//        "added_on", HASH_,
//    };

    /* Appends */
    /*! Map of mutator names to methods. */
    inline static const QHash<QString, MutatorFunction> u_mutators {
        {"name_progress", &Torrent::nameProgress},
        {"name_size",     &Torrent::nameSize},
    };

    /*! The attributes that should be appended during serialization. */
//    std::set<QString> u_appends {
//        "name_progress",
//    };
};

} // namespace Models

/* Recursive #include-s
   ---
   I tried to remove all recursive #include-s beginning from this Torrent class all way
   down and it didn't save any memory during compilation. In both cases with and without
   recursive #include-s the cl.exe memory usage for the tst_model.pro test case was
   2.1GB.
   I also tried to remove all unused relations considering the tst_model.pro test case
   from this Torrent class all way down and it saved a lot of memory, I didn't remember
   exactly this number but the final cl.exe memory usage for the tst_model.pro test case
   was 800MB.
   Now, I tried it again, I removed Tag, Tagged, TorrentState, and User relations,
   #include-s, ... from the Torrent class and also all unused relations from all other
   classes that were used by the tst_model.pro test case and the cl.exe memory usage
   for the tst_model.pro test case was 800MB, when I only left some of these relations,
   eg. in the User or TorrentPreviewableFile models then the memory usage was 1.1GB.
   The conclusion is that recursive #include-s don't increase memory usage during
   compilation (for msvc compiler, other compilers can behave differently but can be
   expected a similar behavior) but model classes themself are increasing memory usage.
   This finding is surprising for me because I thought that these recursive #include-s
   are increasing the memory usage too. */

// TODO finish this, move to base class and test eg in qvector, qhash, etc silverqx
//QDebug operator<<(QDebug debug, const Models::Torrent &c);

#endif // MODELS_TORRENT_HPP
