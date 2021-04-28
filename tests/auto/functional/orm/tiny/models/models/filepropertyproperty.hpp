#ifndef FILEPROPERTYPROPERTY_H
#define FILEPROPERTYPROPERTY_H

#include "orm/tiny/model.hpp"

#include "models/torrentpreviewablefileproperty.hpp"

using Orm::Tiny::Relations::BelongsTo;

class FilePropertyProperty final :
        public Model<FilePropertyProperty, TorrentPreviewableFileProperty>
{
    friend Model;
    using Model::Model;

public:
    /*! Get the previewable file that owns the file property. */
    std::unique_ptr<BelongsTo<FilePropertyProperty, TorrentPreviewableFileProperty>>
    fileProperty()
    {
        return belongsTo<TorrentPreviewableFileProperty>("file_property_id", {},
                                                         __func__);
    }

private:
    /*! The table associated with the model. */
    QString u_table {"file_property_properties"};

    /*! Map of relation names to methods. */
    QHash<QString, RelationVisitor> u_relations {
        {"fileProperty", [](auto &v) { v(&FilePropertyProperty::fileProperty); }},
    };

    /*! The relations to eager load on every query. */
    QVector<WithItem> u_with {
//        {"fileProperty"},
    };

    /*! All of the relationships to be touched. */
    QStringList u_touches {"fileProperty"};
};

#endif // FILEPROPERTYPROPERTY_H
