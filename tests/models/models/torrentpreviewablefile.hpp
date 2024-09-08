#pragma once
#ifndef MODELS_TORRENTPREVIEWABLEFILE_HPP
#define MODELS_TORRENTPREVIEWABLEFILE_HPP

#include "orm/tiny/model.hpp"

namespace Models
{

using Orm::Constants::NAME;
using Orm::Constants::NOTE;
using Orm::Constants::PARENTH_ONE;
using Orm::Constants::Progress;
using Orm::Constants::SIZE_;
using Orm::Constants::SPACE_IN;

using Orm::Tiny::Model;

class Torrent;
class TorrentPreviewableFileProperty;

class TorrentPreviewableFile final : // NOLINT(bugprone-exception-escape, misc-no-recursion)
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
        auto relation = belongsTo<Torrent>(
                            u"torrent_id"_s, {}, QString::fromUtf8(__func__)); // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)

        relation->withDefault();

        return relation;
    }

    /*! Get a torrent that owns the previewable file. */
    std::unique_ptr<BelongsTo<TorrentPreviewableFile, Torrent>>
    torrent_WithVectorDefaults()
    {
        // Ownership of a unique_ptr()
        auto relation = belongsTo<Torrent>(
                            u"torrent_id"_s, {}, QString::fromUtf8(__func__)); // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)

        relation->withDefault({{NAME, u"default_torrent_name"_s}, {SIZE_, 123}});

        return relation;
    }

    /*! Get a file property associated with the previewable file. */
    std::unique_ptr<HasOne<TorrentPreviewableFile, TorrentPreviewableFileProperty>>
    fileProperty()
    {
        return hasOne<TorrentPreviewableFileProperty>(u"previewable_file_id"_s);
    }

    /*! Get a file property associated with the previewable file. */
    std::unique_ptr<HasOne<TorrentPreviewableFile, TorrentPreviewableFileProperty>>
    fileProperty_WithBoolDefault()
    {
        auto relation = hasOne<TorrentPreviewableFileProperty>(u"previewable_file_id"_s);

        relation->withDefault();

        return relation;
    }

    /*! Get a file property associated with the previewable file. */
    std::unique_ptr<HasOne<TorrentPreviewableFile, TorrentPreviewableFileProperty>>
    fileProperty_WithVectorDefaults()
    {
        auto relation = hasOne<TorrentPreviewableFileProperty>(u"previewable_file_id"_s);

        relation->withDefault({{NAME, u"default_fileproperty_name"_s}, {SIZE_, 321}});

        return relation;
    }

protected:
    /*! Accessor for filepath (used in tests to test accessor with existing attribute). */
    Attribute filepath() const noexcept
    {
        return Attribute::make(/* get */ [this]() -> QVariant
        {
            return SPACE_IN.arg(getAttribute<QString>(u"filepath"_s),
                                PARENTH_ONE.arg(u"dummy-STRING"_s));
        });
    }

private:
    /*! Map of relation names to methods. */
    QHash<QString, RelationVisitor> u_relations {
        {u"torrent"_s,                         [](auto &v) { v(&TorrentPreviewableFile::torrent); }},
        {u"torrent_WithBoolDefault"_s,         [](auto &v) { v(&TorrentPreviewableFile::torrent_WithBoolDefault); }},
        {u"torrent_WithVectorDefaults"_s,      [](auto &v) { v(&TorrentPreviewableFile::torrent_WithVectorDefaults); }},
        {u"fileProperty"_s,                    [](auto &v) { v(&TorrentPreviewableFile::fileProperty); }},
        {u"fileProperty_WithBoolDefault"_s,    [](auto &v) { v(&TorrentPreviewableFile::fileProperty_WithBoolDefault); }},
        {u"fileProperty_WithVectorDefaults"_s, [](auto &v) { v(&TorrentPreviewableFile::fileProperty_WithVectorDefaults); }},
    };

    /*! The attributes that are mass assignable. */
    inline static QStringList u_fillable { // NOLINT(cppcoreguidelines-interfaces-global-init)
        u"file_index"_s,
        u"filepath"_s,
        SIZE_,
        Progress,
        NOTE,
    };

    /*! All of the relationships to be touched. */
    QStringList u_touches {u"torrent"_s};

    /* Appends */
    /*! Map of mutator names to methods. */
    inline static const QHash<QString, MutatorFunction> u_mutators {
        {u"filepath"_s, &TorrentPreviewableFile::filepath},
    };
};

} // namespace Models

#endif // MODELS_TORRENTPREVIEWABLEFILE_HPP
