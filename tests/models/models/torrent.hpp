#pragma once
#ifndef MODELS_TORRENT_HPP
#define MODELS_TORRENT_HPP

#ifdef PROJECT_TINYORM_PLAYGROUND
#  include "orm/db.hpp"
#endif

#include "orm/tiny/model.hpp"
//#include "orm/tiny/softdeletes.hpp"

#ifdef PROJECT_TINYORM_PLAYGROUND
#  include "configuration.hpp"
#endif

namespace Models
{

using Orm::Constants::AddedOn;
using Orm::Constants::HASH_;
using Orm::Constants::ID;
using Orm::Constants::NAME;
using Orm::Constants::NOTE;
using Orm::Constants::PARENTH_ONE;
using Orm::Constants::Progress;
using Orm::Constants::SIZE_;
using Orm::Constants::SPACE_IN;

//using Orm::AttributeItem;
//using Orm::QtTimeZoneConfig;

using Orm::Tiny::Model;
using Orm::Tiny::Relations::Pivot;
//using Orm::Tiny::SoftDeletes;

#ifdef PROJECT_TINYORM_PLAYGROUND
using TinyPlay::Configuration;
#endif

/* This class serves as a showcase, so all possible features are defined / used. */

class Tag;
class Tagged;
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

//    explicit Torrent(const QList<AttributeItem> &attributes = {});

    /*! Get previewable files associated with the torrent. */
    std::unique_ptr<HasMany<Torrent, TorrentPreviewableFile>>
    torrentFiles()
    {
        return hasMany<TorrentPreviewableFile>();
//        return hasMany<TorrentPreviewableFile>(u"torrent_id"_s, ID);
    }

    /*! Get a torrent peer associated with the torrent. */
    std::unique_ptr<HasOne<Torrent, TorrentPeer>>
    torrentPeer()
    {
        return hasOne<TorrentPeer>();
//        return hasOne<TorrentPeer>(u"torrent_id"_s, ID);

        // Default Model example
//        auto relation = hasOne<TorrentPeer>();
//        relation->withDefault();
//        relation->withDefault({{u"seeds"_s, 0}, {u"total_seeds"_s, -1}});
        // This callback variant is not yet implemented 😟
//        relation->withDefault([](TorrentPeer &torrentPeer, const Torrent &/*torrent*/)
//        {
//            torrentPeer[NAME] = u"default_peer_name"_s;
//        });
//        return relation;
    }

    /*! Get tags that belong to the torrent. */
    std::unique_ptr<BelongsToMany<Torrent, Tag, Tagged>>
//    std::unique_ptr<BelongsToMany<Torrent, Tag, /*Pivot*/>>
    tags()
    {
        // Basic Pivot model
        // Ownership of a unique_ptr()
//        auto relation = belongsToMany<Tag, /*Pivot*/>();
//        relation->as(u"tagged"_s)
//                 .withPivot(u"active"_s)
//                 .withTimestamps();

        // Custom 'Tagged' pivot model ✨
        // Ownership of a unique_ptr()
        auto relation = belongsToMany<Tag, Tagged>();
        relation->as(u"tagged"_s)
                 .withPivot(u"active"_s)
                 .withTimestamps(/*u"created_at_custom"_s, u"updated_at_custom"_s*/);

        return relation;

        // Basic Pivot model
//        return belongsToMany<Tag>(u"tag_torrent"_s, u"torrent_id"_s, u"tag_id"_s,
//                                  ID, ID, u"tags"_s);
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
        auto relation = belongsToMany<TorrentState>(u"state_torrent"_s, {},
                                                    u"state_id"_s);
        relation->withPivot(u"active"_s);

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
//        return date.toString(u"dd.MM.yyyy"_s);
//    }

    /*! Prepare a datetime for vector, map, or JSON serialization. */
//    inline static QString serializeDateTime(const QDateTime &datetime)
//    {
//        return datetime.toUTC().toString(u"dd.MM.yyyy HH:mm:ss.z t"_s);
//    }

    /* Data members */
    /*! The table associated with the model. */
    QString u_table {u"torrents"_s};

    /*! Indicates if the model's ID is auto-incrementing. */
//    bool u_incrementing = true;
    /*! The primary key associated with the table. */
//    QString u_primaryKey {ID};

    /*! Map of relation names to methods. */
    QHash<QString, RelationVisitor> u_relations {
        {u"torrentFiles"_s,  [](auto &v) { v(&Torrent::torrentFiles); }},
        {u"torrentPeer"_s,   [](auto &v) { v(&Torrent::torrentPeer); }},
        {u"tags"_s,          [](auto &v) { v(&Torrent::tags); }},
        {u"user"_s,          [](auto &v) { v(&Torrent::user); }},
        {u"torrentStates"_s, [](auto &v) { v(&Torrent::torrentStates); }},
    };

    /*! The relations to eager load on every query. */
    QList<QString> u_with { // NOLINT(readability-redundant-member-init)
//        u"torrentFiles"_s,
//        u"torrentPeer"_s,
//        u"torrentFiles.fileProperty"_s,
//        u"tags"_s,
//        u"user"_s,
    };

#ifdef PROJECT_TINYORM_PLAYGROUND
    // I leave the initializer here to be clearly visible
    /*! The connection name for the model. */
    QString u_connection {Orm::DB::getDefaultConnection() == Mysql
                          ? Mysql_Alt
                          : Orm::DB::getDefaultConnection()};
#endif

    /*! The connection name for the model. */
//    QString u_connection {u"sqlite"_s};

    /*! The model's default values for attributes. */
//    inline static const QList<AttributeItem> u_attributes {
//        {SIZE_,      0},
//        {Progress,   0},
//        {AddedOn, QDateTime({2021, 4, 1}, {15, 10, 10}, QtTimeZoneConfig::utc())},
//    };

    /*! The attributes that are mass assignable. */
    inline static const QStringList u_fillable { // NOLINT(cppcoreguidelines-interfaces-global-init)
        ID,
        NAME,
        SIZE_,
        Progress,
        AddedOn,
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
//    inline static QString u_dateFormat {u"yyyy-MM-dd HH:mm:ss"_s};
    /*! The storage format of the model's time columns. */
//    inline static QString u_timeFormat {u"HH:mm:ss"_s};

    /*! The attributes that should be mutated to dates. */
    inline static const QStringList u_dates {AddedOn, u"added_on_alt"_s}; // NOLINT(cppcoreguidelines-interfaces-global-init)

    /*! All of the relationships to be touched. */
//    QStringList u_touches {u"tags"_s};
//    QStringList u_touches {u"relation_name"_s};

    /*! The attributes that should be cast. */
//    inline static std::unordered_map<QString, CastItem> u_casts {
//        {NAME,         CastType::QString},
//        {Progress,     CastType::UShort},
//        // Showcase only, the Torrent model doesn't have the decimal column
//        {u"decimal"_s, CastType::Decimal},
//        {u"decimal"_s, {CastType::Decimal, 2}},
//    };

    /* Serialization */
    /*! Indicates whether attributes are snake_cased during serialization. */
//    inline static const bool u_snakeAttributes = false;

    /* HidesAttributes */
    /*! The attributes that should be visible during serialization. */
//    inline static std::set<QString> u_visible {
//        ID, u"user_id"_s, NAME, SIZE_, Progress, NOTE, CREATED_AT(), UPDATED_AT(),
//    };
    /*! The attributes that should be hidden during serialization. */
//    inline static std::set<QString> u_hidden {
//        AddedOn, HASH_,
//    };

    /* Appends */
    /*! Map of mutator names to methods. */
    inline static const QHash<QString, MutatorFunction> u_mutators {
        {u"name_progress"_s, &Torrent::nameProgress},
        {u"name_size"_s,     &Torrent::nameSize},
    };

    /*! The attributes that should be appended during serialization. */
//    std::set<QString> u_appends {
//        u"name_progress"_s,
//    };
};

} // namespace Models

/* Recursive #include-s
   ---
   New:
   Recursive #include-s were fixed in 99701f4e (see commit message).

   Older findings:
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
   compilation (for MSVC compiler, other compilers can behave differently but can be
   expected a similar behavior) but model classes themself are increasing memory usage.
   This finding is surprising for me because I thought that these recursive #include-s
   are increasing the memory usage too. */

// TODO finish this, move to base class and test eg in qvector, qhash, etc silverqx
//QDebug operator<<(QDebug debug, const Models::Torrent &c);

#endif // MODELS_TORRENT_HPP
