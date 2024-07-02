#pragma once
#ifndef MODELS_TORRENTPREVIEWABLEFILEPROPERTY_HPP
#define MODELS_TORRENTPREVIEWABLEFILEPROPERTY_HPP

#include "orm/tiny/model.hpp"

#include "models/torrentpreviewablefile.hpp"

// Has to be after torrentpreviewablefile.hpp
#include "models/filepropertyproperty.hpp"

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
                    "previewable_file_id", {}, QString::fromUtf8(__func__)); // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    }

    /*! Get a property property associated with the file property. */
    std::unique_ptr<HasMany<TorrentPreviewableFileProperty, FilePropertyProperty>>
    filePropertyProperty()
    {
        return hasMany<FilePropertyProperty>("file_property_id");
    }

private:
    /*! The table associated with the model. */
    QString u_table {"torrent_previewable_file_properties"};

    /*! Map of relation names to methods. */
    QHash<QString, RelationVisitor> u_relations {
        {"torrentFile",          [](auto &v) { v(&TorrentPreviewableFileProperty::torrentFile); }},
        {"filePropertyProperty", [](auto &v) { v(&TorrentPreviewableFileProperty::filePropertyProperty); }},
    };

    /*! The relations to eager load on every query. */
    QList<QString> u_with { // NOLINT(readability-redundant-member-init)
//        "torrentFile",
    };

    /*! Indicates whether the model should be timestamped. */
    bool u_timestamps = false;

    /*! All of the relationships to be touched. */
    QStringList u_touches {"torrentFile"};
};

} // namespace Models

#endif // MODELS_TORRENTPREVIEWABLEFILEPROPERTY_HPP
