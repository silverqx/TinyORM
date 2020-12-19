#ifndef TORRENTPREVIEWABLEFILEPROPERTY_H
#define TORRENTPREVIEWABLEFILEPROPERTY_H

#include "orm/tiny/basemodel.h"

#include "torrentpreviewablefile.h"

class TorrentPreviewableFileProperty final : public Orm::Tiny::BaseModel<TorrentPreviewableFileProperty, TorrentPreviewableFile>
{
public:
    friend class BaseModel;

    explicit TorrentPreviewableFileProperty(const QVector<Orm::AttributeItem> &attributes = {});

    /*! Get the previewable file that owns the file property. */
    std::unique_ptr<
    Orm::Tiny::Relations::Relation<TorrentPreviewableFileProperty, TorrentPreviewableFile>>
    torrentFile()
    {
        return belongsTo<TorrentPreviewableFile>();
    }

private:
    /*! The visitor to obtain a type for Related template parameter. */
    void relationVisitor(const QString &relation)
    {
        if (relation == "torrentFile")
            relationVisited<TorrentPreviewableFile>();
    }

    /*! The table associated with the model. */
    QString u_table {"torrent_previewable_file_properties"};

    /*! Map of relation names to methods. */
    QHash<QString, std::any> u_relations {
        {"torrentFile", &TorrentPreviewableFileProperty::torrentFile},
    };

    /*! The relations to eager load on every query. */
    QVector<Orm::WithItem> u_with {
//        {"torrentFile"},
    };
};

#endif // TORRENTPREVIEWABLEFILEPROPERTY_H
