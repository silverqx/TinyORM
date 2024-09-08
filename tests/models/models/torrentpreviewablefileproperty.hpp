#pragma once
#ifndef MODELS_TORRENTPREVIEWABLEFILEPROPERTY_HPP
#define MODELS_TORRENTPREVIEWABLEFILEPROPERTY_HPP

#include "orm/tiny/model.hpp"

namespace Models
{

using Orm::Tiny::Model;

class FilePropertyProperty;
class TorrentPreviewableFile;

class TorrentPreviewableFileProperty final : // NOLINT(bugprone-exception-escape, misc-no-recursion)
        public Model<TorrentPreviewableFileProperty, TorrentPreviewableFile,
                     FilePropertyProperty>
{
    friend Model;
    using Model::Model;

public:
    /*! Get a previewable file that owns the file property. */
    std::unique_ptr<BelongsTo<TorrentPreviewableFileProperty, TorrentPreviewableFile>>
    torrentFile()
    {
        return belongsTo<TorrentPreviewableFile>(
                    u"previewable_file_id"_s, {}, QString::fromUtf8(__func__)); // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    }

    /*! Get a property property associated with the file property. */
    std::unique_ptr<HasMany<TorrentPreviewableFileProperty, FilePropertyProperty>>
    filePropertyProperty()
    {
        return hasMany<FilePropertyProperty>(u"file_property_id"_s);
    }

private:
    /*! The table associated with the model. */
    QString u_table {u"torrent_previewable_file_properties"_s};

    /*! Map of relation names to methods. */
    QHash<QString, RelationVisitor> u_relations {
        {u"torrentFile"_s,          [](auto &v) { v(&TorrentPreviewableFileProperty::torrentFile); }},
        {u"filePropertyProperty"_s, [](auto &v) { v(&TorrentPreviewableFileProperty::filePropertyProperty); }},
    };

    /*! Indicates whether the model should be timestamped. */
    bool u_timestamps = false; // Timestamps disabled explicitly (even if the table has timestamp columns; they aren't used anywhere)

    /*! All of the relationships to be touched. */
    QStringList u_touches {u"torrentFile"_s};
};

} // namespace Models

#endif // MODELS_TORRENTPREVIEWABLEFILEPROPERTY_HPP
