#ifndef TORRENTPREVIEWABLEFILEEAGER_H
#define TORRENTPREVIEWABLEFILEEAGER_H

#include "orm/tiny/model.hpp"

#include "models/torrentpreviewablefilepropertyeager.hpp"

using Orm::Tiny::Model;
using Orm::Tiny::Relations::HasOne;
using Orm::WithItem;

class TorrentPreviewableFileEager final :
        public Model<TorrentPreviewableFileEager, TorrentPreviewableFilePropertyEager>
{
    friend Model;
    using Model::Model;

public:
    /*! Get the file property associated with the previewable file. */
    std::unique_ptr<HasOne<TorrentPreviewableFileEager,
                           TorrentPreviewableFilePropertyEager>>
    fileProperty()
    {
        return hasOne<TorrentPreviewableFilePropertyEager>("previewable_file_id");
    }

private:
    /*! The table associated with the model. */
    QString u_table {"torrent_previewable_files"};

    /*! Map of relation names to methods. */
    QHash<QString, RelationVisitor> u_relations {
        {"fileProperty", [](auto &v) { v(&TorrentPreviewableFileEager::fileProperty); }},
    };

    /*! The relations to eager load on every query. */
    QVector<WithItem> u_with {
        {"fileProperty"},
    };

    /*! The attributes that are mass assignable. */
    inline static QStringList u_fillable {
        "file_index",
        "filepath",
        "size",
        "progress",
        "note",
    };
};

#endif // TORRENTPREVIEWABLEFILEEAGER_H
