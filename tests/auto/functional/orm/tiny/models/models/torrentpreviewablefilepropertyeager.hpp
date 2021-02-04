#ifndef TORRENTPREVIEWABLEFILEPROPERTYEAGER_H
#define TORRENTPREVIEWABLEFILEPROPERTYEAGER_H

#include "orm/tiny/basemodel.hpp"

#include "models/torrentpreviewablefileeager.hpp"

class TorrentPreviewableFilePropertyEager final :
        public Orm::Tiny::BaseModel<TorrentPreviewableFilePropertyEager,
                                    TorrentPreviewableFileEager>
{
public:
    friend class BaseModel;

    using BaseModel::BaseModel;

    /*! Get the previewable file that owns the file property. */
    std::unique_ptr<
    Orm::Tiny::Relations::Relation<TorrentPreviewableFilePropertyEager,
                                   TorrentPreviewableFileEager>>
    torrentFile()
    {
        return belongsTo<TorrentPreviewableFileEager>("previewable_file_id", {},
                                                      __func__);
    }

private:
    /*! The visitor to obtain a type for Related template parameter. */
    void relationVisitor(const QString &relation)
    {
        if (relation == "torrentFile")
            relationVisited<TorrentPreviewableFileEager>();
    }

    /*! The table associated with the model. */
    QString u_table {"torrent_previewable_file_properties"};

    /*! Map of relation names to methods. */
    QHash<QString, std::any> u_relations {
        {"torrentFile", &TorrentPreviewableFilePropertyEager::torrentFile},
    };

#ifdef PROJECT_TINYORM_TEST
    /*! The connection name for the model. */
    QString u_connection {"tinyorm_mysql_tests"};
#endif
};

#endif // TORRENTPREVIEWABLEFILEPROPERTYEAGER_H
