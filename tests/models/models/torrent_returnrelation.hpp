#pragma once
#ifndef MODELS_TORRENT_RETURNRELATION_HPP
#define MODELS_TORRENT_RETURNRELATION_HPP

#include "orm/tiny/relations/pivot.hpp"

#include "models/tag.hpp"
#include "models/tag_returnrelation.hpp"
#include "models/tagged.hpp"
#include "models/torrentpeer.hpp"
#include "models/torrentpreviewablefile.hpp"
#include "models/user.hpp"

namespace Models
{

using Orm::Tiny::Model;
using Orm::Tiny::Relations::Pivot;

class Tag;
class Tag_ReturnRelation;
class TorrentPeer;
class TorrentPreviewableFile;
class User;

// NOLINTNEXTLINE(misc-no-recursion, bugprone-exception-escape)
class Torrent_ReturnRelation final :
        public Model<Torrent_ReturnRelation, TorrentPreviewableFile, TorrentPeer, Tag,
                     Tag_ReturnRelation, User, Pivot>
{
    friend Model;
    using Model::Model;

public:
    /*! Get previewable files associated with the torrent. */
    std::unique_ptr<Relation<Torrent_ReturnRelation, TorrentPreviewableFile>>
    torrentFiles()
    {
        return hasMany<TorrentPreviewableFile>("torrent_id");
    }

    /*! Get a torrent peer associated with the torrent. */
    std::unique_ptr<Relation<Torrent_ReturnRelation, TorrentPeer>>
    torrentPeer()
    {
        return hasOne<TorrentPeer>("torrent_id");
    }

    /*! Get tags that belong to the torrent. */
    std::unique_ptr<Relation<Torrent_ReturnRelation, Tag_ReturnRelation>>
    tags()
    {
        // Basic pivot model
        // Ownership of a unique_ptr()
        auto relation = belongsToMany<Tag_ReturnRelation>("tag_torrent", "torrent_id",
                                                          "tag_id", {}, {}, "tags");
        relation->withPivot("active")
                 .withTimestamps();

        return relation;
    }

    /*! Get tags that belong to the torrent. */
    std::unique_ptr<Relation<Torrent_ReturnRelation, Tag>>
    tagsCustom()
    {
        // Custom 'Tagged' pivot model ✨
        // Ownership of a unique_ptr()
        auto relation = belongsToMany<Tag, Tagged>("tag_torrent", "torrent_id", {},
                                                   {}, {}, "tags");
        relation->as("tagged")
                 .withPivot("active")
                 .withTimestamps();

        return relation;
    }

    /*! Get a user that owns the torrent. */
    std::unique_ptr<Relation<Torrent_ReturnRelation, User>>
    user()
    {
        return belongsTo<User>({}, {}, QString::fromUtf8(__func__)); // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    }

private:
    /*! The table associated with the model. */
    QString u_table {"torrents"};

    /*! Map of relation names to methods. */
    QHash<QString, RelationVisitor> u_relations {
        {"torrentFiles", [](auto &v) { v(&Torrent_ReturnRelation::torrentFiles); }},
        {"torrentPeer",  [](auto &v) { v(&Torrent_ReturnRelation::torrentPeer); }},
        {"tags",         [](auto &v) { v(&Torrent_ReturnRelation::tags); }},
        {"tagsCustom",   [](auto &v) { v(&Torrent_ReturnRelation::tagsCustom); }},
        {"user",         [](auto &v) { v(&Torrent_ReturnRelation::user); }},
    };

    /*! The attributes that should be mutated to dates. */
    inline static const QStringList u_dates {"added_on"};
};

} // namespace Models

#endif // MODELS_TORRENT_RETURNRELATION_HPP
