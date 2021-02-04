#ifndef TORRENTPREVIEWABLEFILEEAGER_H
#define TORRENTPREVIEWABLEFILEEAGER_H

#include "orm/tiny/basemodel.hpp"

#include "models/torrenteager.hpp"
#include "models/torrentpreviewablefilepropertyeager.hpp"

class TorrentPreviewableFileEager final :
        public Orm::Tiny::BaseModel<TorrentPreviewableFileEager, TorrentEager,
                                    TorrentPreviewableFilePropertyEager>
{
public:
    friend class BaseModel;

    using BaseModel::BaseModel;

    /*! Get the torrent that owns the previewable file. */
    std::unique_ptr<
    Orm::Tiny::Relations::Relation<TorrentPreviewableFileEager, TorrentEager>>
    torrent()
    {
        return belongsTo<TorrentEager>({}, {}, __func__);
    }

    /*! Get the file property associated with the previewable file. */
    std::unique_ptr<
    Orm::Tiny::Relations::Relation<TorrentPreviewableFileEager,
                                   TorrentPreviewableFilePropertyEager>>
    fileProperty()
    {
        return hasOne<TorrentPreviewableFilePropertyEager>("previewable_file_id");
    }

private:
    /*! The visitor to obtain a type for Related template parameter. */
    void relationVisitor(const QString &relation)
    {
        if (relation == "torrent")
            relationVisited<TorrentEager>();
        else if (relation == "fileProperty")
            relationVisited<TorrentPreviewableFilePropertyEager>();
    }

    /*! The table associated with the model. */
    QString u_table {"torrent_previewable_files"};

    /*! Map of relation names to methods. */
    QHash<QString, std::any> u_relations {
        {"torrent",      &TorrentPreviewableFileEager::torrent},
        {"fileProperty", &TorrentPreviewableFileEager::fileProperty},
    };

#ifdef PROJECT_TINYORM_TEST
    /*! The connection name for the model. */
    QString u_connection {"tinyorm_mysql_tests"};
#endif
};

#endif // TORRENTPREVIEWABLEFILEEAGER_H
