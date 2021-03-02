#ifndef TORRENTEAGER_FAILED_H
#define TORRENTEAGER_FAILED_H

#include "orm/tiny/basemodel.hpp"

#include "models/forwardseager.hpp"

#include "models/torrentpreviewablefileeager.hpp"

using Orm::Tiny::BaseModel;
using Orm::Tiny::Relations::Relation;
using Orm::WithItem;

class TorrentEager_Failed final :
        public BaseModel<TorrentEager_Failed, TorrentPreviewableFileEager>
{
    friend BaseModel;
    using BaseModel::BaseModel;

public:
    /*! Get the previewable files associated with the torrent. */
    std::unique_ptr<Relation<TorrentEager_Failed, TorrentPreviewableFileEager>>
    torrentFiles()
    {
        return hasMany<TorrentPreviewableFileEager>("torrent_id");
    }

private:
    /*! The visitor to obtain a type for Related template parameter. */
    void relationVisitor(const QString &relation)
    {
        if (relation == "torrentFiles")
            relationVisited<TorrentPreviewableFileEager>();
    }

    /*! The table associated with the model. */
    QString u_table {"torrents"};

    /*! Map of relation names to methods. */
    QHash<QString, std::any> u_relations {
        {"torrentFiles", &TorrentEager_Failed::torrentFiles},
    };

    /*! The relations to eager load on every query. */
    QVector<WithItem> u_with {
        {"torrentFiles-NON_EXISTENT"},
    };

    /*! The connection name for the model. */
#ifdef PROJECT_TINYORM_TEST
    QString u_connection {"tinyorm_mysql_tests"};
#endif
};

#endif // TORRENTEAGER_FAILED_H
