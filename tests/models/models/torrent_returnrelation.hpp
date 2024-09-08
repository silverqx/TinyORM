#pragma once
#ifndef MODELS_TORRENT_RETURNRELATION_HPP
#define MODELS_TORRENT_RETURNRELATION_HPP

#include "orm/tiny/model.hpp"

namespace Models
{

using Orm::Constants::AddedOn;

using Orm::Tiny::Model;

class Tag_BasicPivot_NoRelations;
class Tag_CustomPivot_NoRelations;
class Tagged;
class TorrentPeer_NoRelations;
class TorrentPreviewableFile_NoRelations;
class User_NoRelations;

class Torrent_ReturnRelation final : // NOLINT(bugprone-exception-escape, misc-no-recursion)
        public Model<Torrent_ReturnRelation, TorrentPreviewableFile_NoRelations,
                     TorrentPeer_NoRelations, Tag_BasicPivot_NoRelations,
                     Tag_CustomPivot_NoRelations, User_NoRelations>
{
    friend Model;
    using Model::Model;

public:
    /*! Get previewable files associated with the torrent. */
    std::unique_ptr<Relation<Torrent_ReturnRelation, TorrentPreviewableFile_NoRelations>>
    torrentFiles()
    {
        return hasMany<TorrentPreviewableFile_NoRelations>(u"torrent_id"_s);
    }

    /*! Get a torrent peer associated with the torrent. */
    std::unique_ptr<Relation<Torrent_ReturnRelation, TorrentPeer_NoRelations>>
    torrentPeer()
    {
        return hasOne<TorrentPeer_NoRelations>(u"torrent_id"_s);
    }

    /*! Get tags that belong to the torrent. */
    std::unique_ptr<Relation<Torrent_ReturnRelation, Tag_BasicPivot_NoRelations>>
    tags()
    {
        // Basic Pivot model
        // Ownership of a unique_ptr()
        auto relation = belongsToMany<Tag_BasicPivot_NoRelations>(
                            u"tag_torrent"_s, u"torrent_id"_s, u"tag_id"_s, {}, {},
                            u"tags"_s);

        relation->withPivot(u"active"_s)
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
                            u"tag_torrent"_s, u"torrent_id"_s, u"tag_id"_s, {}, {},
                            u"tagsCustom"_s);

        relation->as(u"tagged"_s)
                 .withPivot(u"active"_s)
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
    QString u_table {u"torrents"_s};

    /*! Map of relation names to methods. */
    QHash<QString, RelationVisitor> u_relations {
        {u"torrentFiles"_s, [](auto &v) { v(&Torrent_ReturnRelation::torrentFiles); }},
        {u"torrentPeer"_s,  [](auto &v) { v(&Torrent_ReturnRelation::torrentPeer); }},
        {u"tags"_s,         [](auto &v) { v(&Torrent_ReturnRelation::tags); }},
        {u"tagsCustom"_s,   [](auto &v) { v(&Torrent_ReturnRelation::tagsCustom); }},
        {u"user"_s,         [](auto &v) { v(&Torrent_ReturnRelation::user); }},
    };

    /*! The attributes that should be mutated to dates. */
    inline static const QStringList u_dates {AddedOn}; // NOLINT(cppcoreguidelines-interfaces-global-init)
};

} // namespace Models

#endif // MODELS_TORRENT_RETURNRELATION_HPP
