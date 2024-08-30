#pragma once
#ifndef MODELS_TORRENT_RETURNRELATION_HPP
#define MODELS_TORRENT_RETURNRELATION_HPP

#include "orm/tiny/model.hpp"

namespace Models
{

using Orm::Constants::AddedOn;

using Orm::Tiny::Model;
using Orm::Tiny::Relations::Pivot;

class Tag_BasicPivot_NoRelations;
class Tag_CustomPivot_NoRelations;
class Tagged;
class TorrentPeer_NoRelations;
class TorrentPreviewableFile_NoRelations;
class User_NoRelations;

class Torrent_ReturnRelation final : // NOLINT(bugprone-exception-escape, misc-no-recursion)
        public Model<Torrent_ReturnRelation, TorrentPreviewableFile_NoRelations,
                     TorrentPeer_NoRelations, Tag_BasicPivot_NoRelations,
                     Tag_CustomPivot_NoRelations, User_NoRelations, Pivot>
{
    friend Model;
    using Model::Model;

public:
    /*! Get previewable files associated with the torrent. */
    std::unique_ptr<Relation<Torrent_ReturnRelation, TorrentPreviewableFile_NoRelations>>
    torrentFiles()
    {
        return hasMany<TorrentPreviewableFile_NoRelations>("torrent_id");
    }

    /*! Get a torrent peer associated with the torrent. */
    std::unique_ptr<Relation<Torrent_ReturnRelation, TorrentPeer_NoRelations>>
    torrentPeer()
    {
        return hasOne<TorrentPeer_NoRelations>("torrent_id");
    }

    /*! Get tags that belong to the torrent. */
    std::unique_ptr<Relation<Torrent_ReturnRelation, Tag_BasicPivot_NoRelations>>
    tags()
    {
        // Basic Pivot model
        // Ownership of a unique_ptr()
        auto relation = belongsToMany<Tag_BasicPivot_NoRelations>(
                            "tag_torrent", "torrent_id", "tag_id", {}, {}, "tags");

        relation->withPivot("active")
                 .withTimestamps();

        return relation;
    }

    /*! Get tags that belong to the torrent. */
    std::unique_ptr<Relation<Torrent_ReturnRelation, Tag_CustomPivot_NoRelations>>
    tagsCustom()
    {
        // Custom 'Tagged' pivot model ✨
        // Ownership of a unique_ptr()
        auto relation = belongsToMany<Tag_CustomPivot_NoRelations, Tagged>(
                            "tag_torrent", "torrent_id", "tag_id", {}, {}, "tags");

        relation->as("tagged")
                 .withPivot("active")
                 .withTimestamps();

        return relation;
    }

    /*! Get a user that owns the torrent. */
    std::unique_ptr<Relation<Torrent_ReturnRelation, User_NoRelations>>
    user()
    {
        return belongsTo<User_NoRelations>({}, {}, QString::fromUtf8(__func__)); // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
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
    inline static const QStringList u_dates {AddedOn};
};

} // namespace Models

#endif // MODELS_TORRENT_RETURNRELATION_HPP
