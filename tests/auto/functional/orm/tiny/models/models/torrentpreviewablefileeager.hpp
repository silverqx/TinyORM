#ifndef TORRENTPREVIEWABLEFILEEAGER_H
#define TORRENTPREVIEWABLEFILEEAGER_H

#include "orm/tiny/basemodel.hpp"

#include "models/torrentpreviewablefilepropertyeager.hpp"

using Orm::Tiny::BaseModel;
using Orm::Tiny::Relations::Relation;
using Orm::WithItem;

class TorrentPreviewableFileEager final :
        public BaseModel<TorrentPreviewableFileEager, TorrentPreviewableFilePropertyEager>
{
    friend BaseModel;
    using BaseModel::BaseModel;

public:
    /*! Get the file property associated with the previewable file. */
    std::unique_ptr<Relation<TorrentPreviewableFileEager,
                    TorrentPreviewableFilePropertyEager>>
    fileProperty()
    {
        return hasOne<TorrentPreviewableFilePropertyEager>("previewable_file_id");
    }

private:
    /*! The visitor to obtain a type for Related template parameter. */
    void relationVisitor(const QString &relation)
    {
        if (relation == "fileProperty")
            relationVisited<TorrentPreviewableFilePropertyEager>();
    }

    /*! The table associated with the model. */
    QString u_table {"torrent_previewable_files"};

    /*! Map of relation names to methods. */
    QHash<QString, std::any> u_relations {
        {"fileProperty", &TorrentPreviewableFileEager::fileProperty},
    };

    /*! The relations to eager load on every query. */
    QVector<WithItem> u_with {
        {"fileProperty"},
    };
};

#endif // TORRENTPREVIEWABLEFILEEAGER_H
