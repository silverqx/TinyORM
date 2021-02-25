#ifndef TORRENTPREVIEWABLEFILEPROPERTY_H
#define TORRENTPREVIEWABLEFILEPROPERTY_H

#include "orm/tiny/basemodel.hpp"

#include "models/torrentpreviewablefile.hpp"

class TorrentPreviewableFileProperty final :
        public Orm::Tiny::BaseModel<TorrentPreviewableFileProperty,
                                    TorrentPreviewableFile>
{
    friend BaseModel;
    using BaseModel::BaseModel;

public:
    /*! Get the previewable file that owns the file property. */
    std::unique_ptr<
    Orm::Tiny::Relations::Relation<TorrentPreviewableFileProperty,
                                   TorrentPreviewableFile>>
    torrentFile()
    {
        return belongsTo<TorrentPreviewableFile>("previewable_file_id", {}, __func__);
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

#ifdef PROJECT_TINYORM_TEST
    /*! The connection name for the model. */
    QString u_connection {"tinyorm_mysql_tests"};
#endif

    /*! Indicates if the model should be timestamped. */
    bool u_timestamps = false;

    /*! All of the relationships to be touched. */
    QStringList u_touches {"torrentFile"};
};

#endif // TORRENTPREVIEWABLEFILEPROPERTY_H
