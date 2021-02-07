#ifndef TORRENTEAGER_H
#define TORRENTEAGER_H

#include "orm/tiny/basemodel.hpp"

#include "models/forwardseager.hpp"

#include "models/torrentpeereager_norelations.hpp"
#include "models/torrentpreviewablefileeager.hpp"

class TorrentEager final :
        public Orm::Tiny::BaseModel<TorrentEager, TorrentPreviewableFileEager,
                                    TorrentPeerEager_NoRelations>
{
public:
    friend class BaseModel;

    using BaseModel::BaseModel;

    /*! Get the previewable files associated with the torrent. */
    std::unique_ptr<
    Orm::Tiny::Relations::Relation<TorrentEager, TorrentPreviewableFileEager>>
    torrentFiles()
    {
        return hasMany<TorrentPreviewableFileEager>("torrent_id");
    }

    /*! Get the torrent peer associated with the torrent. */
    std::unique_ptr<
    Orm::Tiny::Relations::Relation<TorrentEager, TorrentPeerEager_NoRelations>>
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
    QVector<Orm::WithItem> u_with {
        {"torrentFiles.fileProperty"},
        {"torrentPeer"},
    };

    /*! The connection name for the model. */
#ifdef PROJECT_TINYORM_TEST
    QString u_connection {"tinyorm_mysql_tests"};
#endif
};

#endif // TORRENTEAGER_H
