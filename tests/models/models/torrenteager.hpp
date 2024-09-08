#pragma once
#ifndef MODELS_TORRENTEAGER_HPP
#define MODELS_TORRENTEAGER_HPP

#include "orm/tiny/model.hpp"

namespace Models
{

using Orm::Constants::AddedOn;
using Orm::Constants::HASH_;
using Orm::Constants::NAME;
using Orm::Constants::NOTE;
using Orm::Constants::Progress;
using Orm::Constants::SIZE_;

using Orm::Tiny::Model;

class TorrentPeer_NoRelations;
class TorrentPreviewableFileEager;

class TorrentEager final : public Model<TorrentEager, TorrentPreviewableFileEager, // NOLINT(bugprone-exception-escape, misc-no-recursion)
                                        TorrentPeer_NoRelations>
{
    friend Model;
    using Model::Model;

public:
    /*! Get previewable files associated with the torrent. */
    std::unique_ptr<HasMany<TorrentEager, TorrentPreviewableFileEager>>
    torrentFiles()
    {
        return hasMany<TorrentPreviewableFileEager>(u"torrent_id"_s);
    }

    /*! Get a torrent peer associated with the torrent. */
    std::unique_ptr<HasOne<TorrentEager, TorrentPeer_NoRelations>>
    torrentPeer()
    {
        return hasOne<TorrentPeer_NoRelations>(u"torrent_id"_s);
    }

private:
    /*! The table associated with the model. */
    QString u_table {u"torrents"_s};

    /*! Map of relation names to methods. */
    QHash<QString, RelationVisitor> u_relations {
        {u"torrentFiles"_s, [](auto &v) { v(&TorrentEager::torrentFiles); }},
        {u"torrentPeer"_s,  [](auto &v) { v(&TorrentEager::torrentPeer); }},
    };

    /*! The relations to eager load on every query. */
    QList<QString> u_with {
        u"torrentFiles.fileProperty"_s,
        u"torrentPeer"_s,
    };

    /*! The model's default values for attributes. */
    inline static const QList<AttributeItem> u_attributes { // NOLINT(cppcoreguidelines-interfaces-global-init)
        {SIZE_,      0},
        {Progress,   0},
        {AddedOn,    QDateTime({2021, 4, 1}, {15, 10, 10}, Orm::TTimeZone::UTC)},
    };

    /*! The attributes that are mass assignable. */
    inline static const QStringList u_fillable { // NOLINT(cppcoreguidelines-interfaces-global-init)
        NAME,
        SIZE_,
        Progress,
        AddedOn,
        HASH_,
        NOTE,
    };

    /*! The attributes that should be mutated to dates. */
    inline static const QStringList u_dates {AddedOn};
};

} // namespace Models

#endif // MODELS_TORRENTEAGER_HPP
