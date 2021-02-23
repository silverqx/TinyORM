#ifndef TORRENTPREVIEWABLEFILE_H
#define TORRENTPREVIEWABLEFILE_H

#include "orm/tiny/basemodel.hpp"

#include "models/torrent.hpp"
#include "models/torrentpreviewablefileproperty.hpp"

class TorrentPreviewableFile final :
        public Orm::Tiny::BaseModel<TorrentPreviewableFile, Torrent,
                                    TorrentPreviewableFileProperty>
{
public:
    friend BaseModel;

    using BaseModel::BaseModel;

    /*! Get the torrent that owns the previewable file. */
    std::unique_ptr<
    Orm::Tiny::Relations::Relation<TorrentPreviewableFile, Torrent>>
    torrent()
    {
        return belongsTo<Torrent>();
    }

    /*! Get the file property associated with the previewable file. */
    std::unique_ptr<
    Orm::Tiny::Relations::Relation<TorrentPreviewableFile,
                                   TorrentPreviewableFileProperty>>
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
    QVector<Orm::WithItem> u_with {
//        {"torrent"},
//        {"torrent.torrentPeer"},
//        {"fileProperty"},
    };

#ifdef PROJECT_TINYORM_TEST
    /*! The connection name for the model. */
    QString u_connection {"tinyorm_mysql_tests"};
#endif

    /*! All of the relationships to be touched. */
    QStringList u_touches {"torrent"};
};

#endif // TORRENTPREVIEWABLEFILE_H
