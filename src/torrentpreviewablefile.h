#ifndef TORRENTPREVIEWABLEFILES_H
#define TORRENTPREVIEWABLEFILES_H

#include <QDebug>

#include "torrent.h"
#include "orm/tiny/basemodel.h"
#include "orm/tiny/relations/belongsto.h"

class TorrentPreviewableFile final : public Orm::Tiny::BaseModel<TorrentPreviewableFile, Torrent>
{
public:
    friend class BaseModel;

    TorrentPreviewableFile(const QVector<Orm::AttributeItem> &attributes = {});

    /*! Get the torrent record associated with the user. */
    std::unique_ptr<
    Orm::Tiny::Relations::Relation<TorrentPreviewableFile, Torrent>>
    torrent()
    {
        return belongsTo<Torrent>();
    }

private:
    void eagerVisitor(const QString &relation)
    {
        if (relation == "torrent")
            eagerVisited<Torrent>();
    }

    /*! The table associated with the model. */
    QString u_table {"torrent_previewable_files"};

    /*! Map of relation names to methods. */
    QHash<QString, std::any> u_relations {
        {"torrent", &TorrentPreviewableFile::torrent},
    };

    /*! The relations to eager load on every query. */
    QVector<Orm::WithItem> u_with {
        // TODO detect (best at compile time) circular eager relation problem, exception during this problem is stackoverflow in QRegularExpression silverqx
        {"torrent"},
        // WARNING check behavior (in Eloquent too), when relation exactly like below and how it intere when Torrent class has enabled the same relation TorrentPeer, so TorrentPeer is defined like nested in TorrentPreviewableFile "torrent.torrentPeer" and like normal in Torrent "torrentPeer" silverqx
        {"torrent.torrentPeer"},
    };
};

#endif // TORRENTPREVIEWABLEFILES_H
