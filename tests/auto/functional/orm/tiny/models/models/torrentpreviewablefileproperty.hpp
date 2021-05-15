#ifndef TORRENTPREVIEWABLEFILEPROPERTY_H
#define TORRENTPREVIEWABLEFILEPROPERTY_H

#include "orm/tiny/model.hpp"

#include "models/torrentpreviewablefile.hpp"

class TorrentPreviewableFile;

class TorrentPreviewableFileProperty final :
        public Model<TorrentPreviewableFileProperty, TorrentPreviewableFile>
{
    friend Model;
    using Model::Model;

public:
    /*! Get the previewable file that owns the file property. */
    std::unique_ptr<BelongsTo<TorrentPreviewableFileProperty, TorrentPreviewableFile>>
    torrentFile()
    {
        return belongsTo<TorrentPreviewableFile>("previewable_file_id", {}, __func__);
    }

private:
    /*! The table associated with the model. */
    QString u_table {"torrent_previewable_file_properties"};

    /*! Map of relation names to methods. */
    QHash<QString, RelationVisitor> u_relations {
        {"torrentFile", [](auto &v) { v(&TorrentPreviewableFileProperty::torrentFile); }},
    };

    /*! The relations to eager load on every query. */
    QVector<QString> u_with {
//        "torrentFile",
    };

    /*! Indicates if the model should be timestamped. */
    bool u_timestamps = false;

    /*! All of the relationships to be touched. */
    QStringList u_touches {"torrentFile"};
};

#endif // TORRENTPREVIEWABLEFILEPROPERTY_H
