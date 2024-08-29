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
                            "torrent_id", {}, QString::fromUtf8(__func__)); // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)

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
                            "torrent_id", {}, QString::fromUtf8(__func__)); // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)

        relation->withDefault({{NAME, "default_torrent_name"}, {SIZE_, 123}});

        return relation;
    }

    /*! Get a file property associated with the previewable file. */
    std::unique_ptr<HasOne<TorrentPreviewableFileEager_WithDefault,
                           TorrentPreviewableFileProperty_NoRelations>>
    fileProperty()
    {
        return hasOne<TorrentPreviewableFileProperty_NoRelations>("previewable_file_id");
    }

    /*! Get a file property associated with the previewable file. */
    std::unique_ptr<HasOne<TorrentPreviewableFileEager_WithDefault,
                           TorrentPreviewableFileProperty_NoRelations>>
    fileProperty_WithBoolDefault()
    {
        auto relation = hasOne<TorrentPreviewableFileProperty_NoRelations>(
                            "previewable_file_id");

        relation->withDefault();

        return relation;
    }

    /*! Get a file property associated with the previewable file. */
    std::unique_ptr<HasOne<TorrentPreviewableFileEager_WithDefault,
                           TorrentPreviewableFileProperty_NoRelations>>
    fileProperty_WithVectorDefaults()
    {
        auto relation = hasOne<TorrentPreviewableFileProperty_NoRelations>(
                            "previewable_file_id");

        relation->withDefault({{NAME, "default_fileproperty_name"}, {SIZE_, 321}});

        return relation;
    }

private:
    /*! The table associated with the model. */
    QString u_table {"torrent_previewable_files"};

    /*! Map of relation names to methods. */
    QHash<QString, RelationVisitor> u_relations {
        {"torrent",                         [](auto &v) { v(&TorrentPreviewableFileEager_WithDefault::torrent); }},
        {"torrent_WithBoolDefault",         [](auto &v) { v(&TorrentPreviewableFileEager_WithDefault::torrent_WithBoolDefault); }},
        {"torrent_WithVectorDefaults",      [](auto &v) { v(&TorrentPreviewableFileEager_WithDefault::torrent_WithVectorDefaults); }},
        {"fileProperty",                    [](auto &v) { v(&TorrentPreviewableFileEager_WithDefault::fileProperty); }},
        {"fileProperty_WithBoolDefault",    [](auto &v) { v(&TorrentPreviewableFileEager_WithDefault::fileProperty_WithBoolDefault); }},
        {"fileProperty_WithVectorDefaults", [](auto &v) { v(&TorrentPreviewableFileEager_WithDefault::fileProperty_WithVectorDefaults); }},
    };

    /*! The relations to eager load on every query. */
    QList<QString> u_with {
        "torrent",
        "torrent_WithBoolDefault",
        "torrent_WithVectorDefaults",
        "fileProperty",
        "fileProperty_WithBoolDefault",
        "fileProperty_WithVectorDefaults",
    };

    /*! The attributes that are mass assignable. */
    inline static QStringList u_fillable { // NOLINT(cppcoreguidelines-interfaces-global-init)
        "file_index",
        "filepath",
        SIZE_,
        Progress,
        NOTE,
    };
};

} // namespace Models

#endif // MODELS_TORRENTPREVIEWABLEFILEEAGER_WITHDEFAULT_HPP
