#ifndef TORRENTPREVIEWABLEFILE_H
#define TORRENTPREVIEWABLEFILE_H

#include "orm/tiny/model.hpp"

#include "models/torrent.hpp"
#include "models/torrentpreviewablefileproperty.hpp"

using Orm::Tiny::Relations::BelongsTo;
using Orm::Tiny::Relations::HasOne;

class Torrent;
class TorrentPreviewableFileProperty;

class TorrentPreviewableFile final :
        public Model<TorrentPreviewableFile, Torrent, TorrentPreviewableFileProperty>
{
    friend Model;
    using Model::Model;

public:
    /*! Get the torrent that owns the previewable file. */
    std::unique_ptr<BelongsTo<TorrentPreviewableFile, Torrent>>
    torrent()
    {
        return belongsTo<Torrent>();
    }

    /*! Get the torrent that owns the previewable file. */
    std::unique_ptr<BelongsTo<TorrentPreviewableFile, Torrent>>
    torrent_WithBoolDefault()
    {
        // Ownership of a unique_ptr()
        auto relation = belongsTo<Torrent>();

        relation->withDefault();

        return relation;
    }

    /*! Get the torrent that owns the previewable file. */
    std::unique_ptr<BelongsTo<TorrentPreviewableFile, Torrent>>
    torrent_WithVectorDefaults()
    {
        // Ownership of a unique_ptr()
        auto relation = belongsTo<Torrent>();

        relation->withDefault({{"name", "default_torrent_name"}, {"size", 123}});

        return relation;
    }

    /*! Get the file property associated with the previewable file. */
    std::unique_ptr<HasOne<TorrentPreviewableFile, TorrentPreviewableFileProperty>>
    fileProperty()
    {
        return hasOne<TorrentPreviewableFileProperty>("previewable_file_id");
    }

    /*! Get the file property associated with the previewable file. */
    std::unique_ptr<HasOne<TorrentPreviewableFile, TorrentPreviewableFileProperty>>
    fileProperty_WithBoolDefault()
    {
        auto relation = hasOne<TorrentPreviewableFileProperty>("previewable_file_id");

        relation->withDefault();

        return relation;
    }

    /*! Get the file property associated with the previewable file. */
    std::unique_ptr<HasOne<TorrentPreviewableFile, TorrentPreviewableFileProperty>>
    fileProperty_WithVectorDefaults()
    {
        auto relation = hasOne<TorrentPreviewableFileProperty>("previewable_file_id");

        relation->withDefault({{"name", "default_fileproperty_name"}, {"size", 321}});

        return relation;
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
    inline static QStringList u_fillable {
        "file_index",
        "filepath",
        "size",
        "progress",
        "note",
    };

    /*! All of the relationships to be touched. */
    QStringList u_touches {"torrent"};
};

#endif // TORRENTPREVIEWABLEFILE_H
