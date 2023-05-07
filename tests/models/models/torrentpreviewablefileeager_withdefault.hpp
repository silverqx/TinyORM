#pragma once
#ifndef MODELS_TORRENTPREVIEWABLEFILEEAGER_WITHDEFAULT_HPP
#define MODELS_TORRENTPREVIEWABLEFILEEAGER_WITHDEFAULT_HPP

#include "orm/tiny/model.hpp"

#include "models/torrenteager_withdefault.hpp"
#include "models/torrentpreviewablefilepropertyeager.hpp"

namespace Models
{

using Orm::Constants::NAME;
using Orm::Constants::NOTE;
using Orm::Constants::SIZE_;

using Orm::Tiny::Model;
using Orm::Tiny::Relations::BelongsTo;
using Orm::Tiny::Relations::HasOne;

// NOLINTNEXTLINE(bugprone-exception-escape)
class TorrentPreviewableFileEager_WithDefault final :
        public Model<TorrentPreviewableFileEager_WithDefault, TorrentEager_WithDefault,
                     TorrentPreviewableFilePropertyEager>
{
    friend Model;
    using Model::Model;

public:
    /*! Get a torrent that owns the previewable file. */
    std::unique_ptr<BelongsTo<TorrentPreviewableFileEager_WithDefault,
                              TorrentEager_WithDefault>>
    torrent()
    {
        return belongsTo<TorrentEager_WithDefault>({}, {}, QString::fromUtf8(__func__)); // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    }

    /*! Get a torrent that owns the previewable file. */
    std::unique_ptr<BelongsTo<TorrentPreviewableFileEager_WithDefault,
                              TorrentEager_WithDefault>>
    torrent_WithBoolDefault()
    {
        // Ownership of a unique_ptr()
        auto relation = belongsTo<TorrentEager_WithDefault>(
                            {}, {}, QString::fromUtf8(__func__)); // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)

        relation->withDefault();

        return relation;
    }

    /*! Get a torrent that owns the previewable file. */
    std::unique_ptr<BelongsTo<TorrentPreviewableFileEager_WithDefault,
                              TorrentEager_WithDefault>>
    torrent_WithVectorDefaults()
    {
        // Ownership of a unique_ptr()
        auto relation = belongsTo<TorrentEager_WithDefault>(
                            {}, {}, QString::fromUtf8(__func__)); // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)

        relation->withDefault({{NAME, "default_torrent_name"}, {SIZE_, 123}});

        return relation;
    }

    /*! Get a file property associated with the previewable file. */
    std::unique_ptr<HasOne<TorrentPreviewableFileEager_WithDefault,
                           TorrentPreviewableFilePropertyEager>>
    fileProperty()
    {
        return hasOne<TorrentPreviewableFilePropertyEager>("previewable_file_id");
    }

    /*! Get a file property associated with the previewable file. */
    std::unique_ptr<HasOne<TorrentPreviewableFileEager_WithDefault,
                           TorrentPreviewableFilePropertyEager>>
    fileProperty_WithBoolDefault()
    {
        auto relation =
                hasOne<TorrentPreviewableFilePropertyEager>("previewable_file_id");

        relation->withDefault();

        return relation;
    }

    /*! Get a file property associated with the previewable file. */
    std::unique_ptr<HasOne<TorrentPreviewableFileEager_WithDefault,
                           TorrentPreviewableFilePropertyEager>>
    fileProperty_WithVectorDefaults()
    {
        auto relation =
                hasOne<TorrentPreviewableFilePropertyEager>("previewable_file_id");

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
    QVector<QString> u_with {
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
        "progress",
        NOTE,
    };
};

} // namespace Models

#endif // MODELS_TORRENTPREVIEWABLEFILEEAGER_WITHDEFAULT_HPP
