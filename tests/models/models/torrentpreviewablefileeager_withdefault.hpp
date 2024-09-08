#pragma once
#ifndef MODELS_TORRENTPREVIEWABLEFILEEAGER_WITHDEFAULT_HPP
#define MODELS_TORRENTPREVIEWABLEFILEEAGER_WITHDEFAULT_HPP

#include "orm/tiny/model.hpp"

namespace Models
{

using Orm::Constants::NAME;
using Orm::Constants::NOTE;
using Orm::Constants::Progress;
using Orm::Constants::SIZE_;

using Orm::Tiny::Model;

class Torrent_NoRelations;
class TorrentPreviewableFileProperty_NoRelations;

class TorrentPreviewableFileEager_WithDefault final : // NOLINT(bugprone-exception-escape, misc-no-recursion)
        public Model<TorrentPreviewableFileEager_WithDefault, Torrent_NoRelations,
                     TorrentPreviewableFileProperty_NoRelations>
{
    friend Model;
    using Model::Model;

public:
    /*! Get a torrent that owns the previewable file. */
    std::unique_ptr<BelongsTo<TorrentPreviewableFileEager_WithDefault,
                              Torrent_NoRelations>>
    torrent()
    {
        return belongsTo<Torrent_NoRelations>({}, {}, QString::fromUtf8(__func__)); // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    }

    /*! Get a torrent that owns the previewable file. */
    std::unique_ptr<BelongsTo<TorrentPreviewableFileEager_WithDefault,
                              Torrent_NoRelations>>
    torrent_WithBoolDefault()
    {
        // Ownership of a unique_ptr()
        auto relation = belongsTo<Torrent_NoRelations>(
                            u"torrent_id"_s, {}, QString::fromUtf8(__func__)); // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)

        relation->withDefault();

        return relation;
    }

    /*! Get a torrent that owns the previewable file. */
    std::unique_ptr<BelongsTo<TorrentPreviewableFileEager_WithDefault,
                              Torrent_NoRelations>>
    torrent_WithVectorDefaults()
    {
        // Ownership of a unique_ptr()
        auto relation = belongsTo<Torrent_NoRelations>(
                            u"torrent_id"_s, {}, QString::fromUtf8(__func__)); // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)

        relation->withDefault({{NAME, u"default_torrent_name"_s}, {SIZE_, 123}});

        return relation;
    }

    /*! Get a file property associated with the previewable file. */
    std::unique_ptr<HasOne<TorrentPreviewableFileEager_WithDefault,
                           TorrentPreviewableFileProperty_NoRelations>>
    fileProperty()
    {
        return hasOne<TorrentPreviewableFileProperty_NoRelations>(
                    u"previewable_file_id"_s);
    }

    /*! Get a file property associated with the previewable file. */
    std::unique_ptr<HasOne<TorrentPreviewableFileEager_WithDefault,
                           TorrentPreviewableFileProperty_NoRelations>>
    fileProperty_WithBoolDefault()
    {
        auto relation = hasOne<TorrentPreviewableFileProperty_NoRelations>(
                            u"previewable_file_id"_s);

        relation->withDefault();

        return relation;
    }

    /*! Get a file property associated with the previewable file. */
    std::unique_ptr<HasOne<TorrentPreviewableFileEager_WithDefault,
                           TorrentPreviewableFileProperty_NoRelations>>
    fileProperty_WithVectorDefaults()
    {
        auto relation = hasOne<TorrentPreviewableFileProperty_NoRelations>(
                            u"previewable_file_id"_s);

        relation->withDefault({{NAME, u"default_fileproperty_name"_s}, {SIZE_, 321}});

        return relation;
    }

private:
    /*! The table associated with the model. */
    QString u_table {u"torrent_previewable_files"_s};

    /*! Map of relation names to methods. */
    QHash<QString, RelationVisitor> u_relations {
        {u"torrent"_s,                         [](auto &v) { v(&TorrentPreviewableFileEager_WithDefault::torrent); }},
        {u"torrent_WithBoolDefault"_s,         [](auto &v) { v(&TorrentPreviewableFileEager_WithDefault::torrent_WithBoolDefault); }},
        {u"torrent_WithVectorDefaults"_s,      [](auto &v) { v(&TorrentPreviewableFileEager_WithDefault::torrent_WithVectorDefaults); }},
        {u"fileProperty"_s,                    [](auto &v) { v(&TorrentPreviewableFileEager_WithDefault::fileProperty); }},
        {u"fileProperty_WithBoolDefault"_s,    [](auto &v) { v(&TorrentPreviewableFileEager_WithDefault::fileProperty_WithBoolDefault); }},
        {u"fileProperty_WithVectorDefaults"_s, [](auto &v) { v(&TorrentPreviewableFileEager_WithDefault::fileProperty_WithVectorDefaults); }},
    };

    /*! The relations to eager load on every query. */
    QList<QString> u_with {
        u"torrent"_s,
        u"torrent_WithBoolDefault"_s,
        u"torrent_WithVectorDefaults"_s,
        u"fileProperty"_s,
        u"fileProperty_WithBoolDefault"_s,
        u"fileProperty_WithVectorDefaults"_s,
    };

    /*! The attributes that are mass assignable. */
    inline static QStringList u_fillable { // NOLINT(cppcoreguidelines-interfaces-global-init)
        u"file_index"_s,
        u"filepath"_s,
        SIZE_,
        Progress,
        NOTE,
    };
};

} // namespace Models

#endif // MODELS_TORRENTPREVIEWABLEFILEEAGER_WITHDEFAULT_HPP
