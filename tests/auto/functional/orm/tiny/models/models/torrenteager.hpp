#ifndef TORRENTEAGER_H
#define TORRENTEAGER_H

#include "orm/tiny/basemodel.hpp"

#include "models/forwardseager.hpp"

#include "models/torrentpeereager_norelations.hpp"
#include "models/torrentpreviewablefileeager.hpp"

using Orm::AttributeItem;
using Orm::Tiny::BaseModel;
using Orm::Tiny::Relations::Relation;
using Orm::WithItem;

class TorrentEager final :
        public BaseModel<TorrentEager, TorrentPreviewableFileEager,
                         TorrentPeerEager_NoRelations>
{
    friend BaseModel;
    using BaseModel::BaseModel;

public:
    /*! Get the previewable files associated with the torrent. */
    std::unique_ptr<Relation<TorrentEager, TorrentPreviewableFileEager>>
    torrentFiles()
    {
        return hasMany<TorrentPreviewableFileEager>("torrent_id");
    }

    /*! Get the torrent peer associated with the torrent. */
    std::unique_ptr<Relation<TorrentEager, TorrentPeerEager_NoRelations>>
    torrentPeer()
    {
        return hasOne<TorrentPeerEager_NoRelations>("torrent_id");
    }

private:
    /*! The visitor to obtain a type for Related template parameter. */
    void relationVisitor(const QString &relation)
    {
        if (relation == "torrentFiles")
            relationVisited<TorrentPreviewableFileEager>();
        else if (relation == "torrentPeer")
            relationVisited<TorrentPeerEager_NoRelations>();
    }

    /*! The table associated with the model. */
    QString u_table {"torrents"};

    /*! Map of relation names to methods. */
    QHash<QString, std::any> u_relations {
        {"torrentFiles", &TorrentEager::torrentFiles},
        {"torrentPeer",  &TorrentEager::torrentPeer},
    };

    /*! The relations to eager load on every query. */
    QVector<WithItem> u_with {
        {"torrentFiles.fileProperty"},
        {"torrentPeer"},
    };

    /*! The model's default values for attributes. */
    inline static const QVector<AttributeItem> u_attributes {
        {"size",     0},
        {"progress", 0},
        {"added_on", QDateTime::fromString("2021-04-01 15:10:10", Qt::ISODate)},
    };

    /*! The attributes that are mass assignable. */
    inline static QStringList u_fillable {
        "name",
        "size",
        "progress",
        "added_on",
        "hash",
        "note",
    };

    /*! The attributes that should be mutated to dates. @deprecated */
    inline static QStringList u_dates {"added_on"};
};

#endif // TORRENTEAGER_H
