#ifndef TORRENTEAGER_FAILED_H
#define TORRENTEAGER_FAILED_H

#include "orm/tiny/model.hpp"

#include "models/forwardseager.hpp"

#include "models/torrentpreviewablefileeager.hpp"

using Orm::Tiny::Model;
using Orm::Tiny::Relations::Relation;
using Orm::WithItem;

class TorrentEager_Failed final :
        public Model<TorrentEager_Failed, TorrentPreviewableFileEager>
{
    friend Model;
    using Model::Model;

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

    /*! The attributes that should be mutated to dates. @deprecated */
    inline static QStringList u_dates {"added_on"};
};

#endif // TORRENTEAGER_FAILED_H
