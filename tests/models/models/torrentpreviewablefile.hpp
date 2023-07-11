#pragma once
#ifndef MODELS_TORRENTPREVIEWABLEFILE_HPP
#define MODELS_TORRENTPREVIEWABLEFILE_HPP

#include "orm/tiny/model.hpp"

#include "models/torrent.hpp"
#include "models/torrentpreviewablefileproperty.hpp"

namespace Models
{

using Orm::Constants::NAME;
using Orm::Constants::NOTE;
using Orm::Constants::PARENTH_ONE;
using Orm::Constants::Progress;
using Orm::Constants::SIZE_;
using Orm::Constants::SPACE_IN;

using Orm::Tiny::Casts::Attribute;
using Orm::Tiny::Relations::BelongsTo;
using Orm::Tiny::Relations::HasOne;

class Torrent;
class TorrentPreviewableFileProperty;

// NOLINTNEXTLINE(misc-no-recursion, bugprone-exception-escape)
class TorrentPreviewableFile final :
        public Model<TorrentPreviewableFile, Torrent, TorrentPreviewableFileProperty>
{
    friend Model;
    using Model::Model;

public:
    /*! Get a torrent that owns the previewable file. */
    std::unique_ptr<BelongsTo<TorrentPreviewableFile, Torrent>>
    torrent()
    {
        return belongsTo<Torrent>();
    }

    /*! Get a torrent that owns the previewable file. */
    std::unique_ptr<BelongsTo<TorrentPreviewableFile, Torrent>>
    torrent_WithBoolDefault()
    {
        // Ownership of a unique_ptr()
        auto relation = belongsTo<Torrent>();

        relation->withDefault();

        return relation;
    }

    /*! Get a torrent that owns the previewable file. */
    std::unique_ptr<BelongsTo<TorrentPreviewableFile, Torrent>>
    torrent_WithVectorDefaults()
    {
        // Ownership of a unique_ptr()
        auto relation = belongsTo<Torrent>();

        relation->withDefault({{NAME, "default_torrent_name"}, {SIZE_, 123}});

        return relation;
    }

    /*! Get a file property associated with the previewable file. */
    std::unique_ptr<HasOne<TorrentPreviewableFile, TorrentPreviewableFileProperty>>
    fileProperty()
    {
        return hasOne<TorrentPreviewableFileProperty>("previewable_file_id");
    }

    /*! Get a file property associated with the previewable file. */
    std::unique_ptr<HasOne<TorrentPreviewableFile, TorrentPreviewableFileProperty>>
    fileProperty_WithBoolDefault()
    {
        auto relation = hasOne<TorrentPreviewableFileProperty>("previewable_file_id");

        relation->withDefault();

        return relation;
    }

    /*! Get a file property associated with the previewable file. */
    std::unique_ptr<HasOne<TorrentPreviewableFile, TorrentPreviewableFileProperty>>
    fileProperty_WithVectorDefaults()
    {
        auto relation = hasOne<TorrentPreviewableFileProperty>("previewable_file_id");

        relation->withDefault({{NAME, "default_fileproperty_name"}, {SIZE_, 321}});

        return relation;
    }

protected:
    /*! Accessor for filepath (used in tests to test accessor with existing attribute). */
    Attribute filepath() const noexcept
    {
        return Attribute::make(/* get */ [this]() -> QVariant
        {
            return SPACE_IN.arg(getAttribute<QString>("filepath"),
                                PARENTH_ONE.arg("dummy-STRING"));
        });
    }

private:
    /*! The table associated with the model. */
    QString u_table {"torrent_previewable_files"};

    /*! Map of relation names to methods. */
    QHash<QString, RelationVisitor> u_relations {
        {"torrent",                         [](auto &v) { v(&TorrentPreviewableFile::torrent); }},
        {"torrent_WithBoolDefault",         [](auto &v) { v(&TorrentPreviewableFile::torrent_WithBoolDefault); }},
        {"torrent_WithVectorDefaults",      [](auto &v) { v(&TorrentPreviewableFile::torrent_WithVectorDefaults); }},
        {"fileProperty",                    [](auto &v) { v(&TorrentPreviewableFile::fileProperty); }},
        {"fileProperty_WithBoolDefault",    [](auto &v) { v(&TorrentPreviewableFile::fileProperty_WithBoolDefault); }},
        {"fileProperty_WithVectorDefaults", [](auto &v) { v(&TorrentPreviewableFile::fileProperty_WithVectorDefaults); }},
    };

    /*! The relations to eager load on every query. */
    QVector<QString> u_with {
//        "torrent",
//        "torrent.torrentPeer",
//        "fileProperty",
    };

    /*! The attributes that are mass assignable. */
    inline static QStringList u_fillable { // NOLINT(cppcoreguidelines-interfaces-global-init)
        "file_index",
        "filepath",
        SIZE_,
        Progress,
        NOTE,
    };

    /*! All of the relationships to be touched. */
    QStringList u_touches {"torrent"};

    /* Appends */
    /*! Map of mutator names to methods. */
    inline static const QHash<QString, MutatorFunction> u_mutators {
        {"filepath", &TorrentPreviewableFile::filepath},
    };
};

} // namespace Models

#endif // MODELS_TORRENTPREVIEWABLEFILE_HPP
