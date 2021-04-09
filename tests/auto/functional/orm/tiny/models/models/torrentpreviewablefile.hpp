#ifndef TORRENTPREVIEWABLEFILE_H
#define TORRENTPREVIEWABLEFILE_H

#include "orm/tiny/basemodel.hpp"

#include "models/torrent.hpp"
#include "models/torrentpreviewablefileproperty.hpp"

class TorrentPreviewableFile final :
        public BaseModel<TorrentPreviewableFile, Torrent, TorrentPreviewableFileProperty>
{
    friend BaseModel;
    using BaseModel::BaseModel;

public:
    /*! Get the torrent that owns the previewable file. */
    std::unique_ptr<Relation<TorrentPreviewableFile, Torrent>>
    torrent()
    {
        return belongsTo<Torrent>();
    }

    /*! Get the file property associated with the previewable file. */
    std::unique_ptr<Relation<TorrentPreviewableFile, TorrentPreviewableFileProperty>>
    fileProperty()
    {
        return hasOne<TorrentPreviewableFileProperty>("previewable_file_id");
    }

private:
    /*! The visitor to obtain a type for Related template parameter. */
    void relationVisitor(const QString &relation)
    {
        if (relation      == "torrent")
            relationVisited<Torrent>();
        else if (relation == "fileProperty")
            relationVisited<TorrentPreviewableFileProperty>();
    }

    /*! The table associated with the model. */
    QString u_table {"torrent_previewable_files"};

    /*! Map of relation names to methods. */
    QHash<QString, std::any> u_relations {
        {"torrent",      &TorrentPreviewableFile::torrent},
        {"fileProperty", &TorrentPreviewableFile::fileProperty},
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
