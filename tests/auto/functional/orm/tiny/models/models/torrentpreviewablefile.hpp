#ifndef TORRENTPREVIEWABLEFILE_H
#define TORRENTPREVIEWABLEFILE_H

#include "orm/tiny/model.hpp"

#include "models/torrent.hpp"
#include "models/torrentpreviewablefileproperty.hpp"

class TorrentPreviewableFile final :
        public Model<TorrentPreviewableFile, Torrent, TorrentPreviewableFileProperty>
{
    friend Model;
    using Model::Model;

public:
    /*! Get the torrent that owns the previewable file. */
    std::unique_ptr<Relation<TorrentPreviewableFile, Torrent>>
    torrent()
    {
        return belongsTo<Torrent>();
    }

    /*! Get the torrent that owns the previewable file. */
    std::unique_ptr<Relation<TorrentPreviewableFile, Torrent>>
    torrent_WithBoolDefault()
    {
        // Ownership of a unique_ptr()
        auto relation = belongsTo<Torrent>();

        relation->withDefault();

        return relation;
    }

    /*! Get the torrent that owns the previewable file. */
    std::unique_ptr<Relation<TorrentPreviewableFile, Torrent>>
    torrent_WithVectorDefaults()
    {
        // Ownership of a unique_ptr()
        auto relation = belongsTo<Torrent>();

        relation->withDefault({{"name", "default_torrent_name"}, {"size", 123}});

        return relation;
    }

    /*! Get the file property associated with the previewable file. */
    std::unique_ptr<Relation<TorrentPreviewableFile, TorrentPreviewableFileProperty>>
    fileProperty()
    {
        return hasOne<TorrentPreviewableFileProperty>("previewable_file_id");
    }

    /*! Get the file property associated with the previewable file. */
    std::unique_ptr<Relation<TorrentPreviewableFile, TorrentPreviewableFileProperty>>
    fileProperty_WithBoolDefault()
    {
        auto relation = hasOne<TorrentPreviewableFileProperty>("previewable_file_id");

        relation->withDefault();

        return relation;
    }

    /*! Get the file property associated with the previewable file. */
    std::unique_ptr<Relation<TorrentPreviewableFile, TorrentPreviewableFileProperty>>
    fileProperty_WithVectorDefaults()
    {
        auto relation = hasOne<TorrentPreviewableFileProperty>("previewable_file_id");

        relation->withDefault({{"name", "default_fileproperty_name"}, {"size", 321}});

        return relation;
    }

private:
    /*! The visitor to obtain a type for Related template parameter. */
    void relationVisitor(const QString &relation)
    {
        if (QStringList{"torrent",
                        "torrent_WithBoolDefault",
                        "torrent_WithVectorDefaults"}.contains(relation))
            relationVisited<Torrent>();
        else if (QStringList{"fileProperty",
                             "fileProperty_WithBoolDefault",
                             "fileProperty_WithVectorDefaults"}.contains(relation))
            relationVisited<TorrentPreviewableFileProperty>();
    }

    /*! The table associated with the model. */
    QString u_table {"torrent_previewable_files"};

    /*! Map of relation names to methods. */
    QHash<QString, std::any> u_relations {
        {"torrent",                         &TorrentPreviewableFile::torrent},
        {"torrent_WithBoolDefault",         &TorrentPreviewableFile::torrent_WithBoolDefault},
        {"torrent_WithVectorDefaults",      &TorrentPreviewableFile::torrent_WithVectorDefaults},
        {"fileProperty",                    &TorrentPreviewableFile::fileProperty},
        {"fileProperty_WithBoolDefault",    &TorrentPreviewableFile::fileProperty_WithBoolDefault},
        {"fileProperty_WithVectorDefaults", &TorrentPreviewableFile::fileProperty_WithVectorDefaults},
    };

    /*! The relations to eager load on every query. */
    QVector<WithItem> u_with {
//        {"torrent"},
//        {"torrent.torrentPeer"},
//        {"fileProperty"},
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
