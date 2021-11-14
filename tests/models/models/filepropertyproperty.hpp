#pragma once
#ifndef MODELS_FILEPROPERTYPROPERTY_HPP
#define MODELS_FILEPROPERTYPROPERTY_HPP

#include "orm/tiny/model.hpp"

#include "models/torrentpreviewablefileproperty.hpp"

namespace Models
{

using Orm::Tiny::Relations::BelongsTo;

class TorrentPreviewableFileProperty;

// NOLINTNEXTLINE(misc-no-recursion)
class FilePropertyProperty final :
        public Model<FilePropertyProperty, TorrentPreviewableFileProperty>
{
    friend Model;
    using Model::Model;

public:
    /*! Get a previewable file that owns the file property. */
    std::unique_ptr<BelongsTo<FilePropertyProperty, TorrentPreviewableFileProperty>>
    fileProperty()
    {
        return belongsTo<TorrentPreviewableFileProperty>(
                    "file_property_id", {}, static_cast<const char *>(__func__));
    }

private:
    /*! The table associated with the model. */
    QString u_table {"file_property_properties"};

    /*! Map of relation names to methods. */
    QHash<QString, RelationVisitor> u_relations {
        {"fileProperty", [](auto &v) { v(&FilePropertyProperty::fileProperty); }},
    };

    /*! The relations to eager load on every query. */
    QVector<QString> u_with {
//        "fileProperty",
    };

    /*! All of the relationships to be touched. */
    QStringList u_touches {"fileProperty"};
};

} // namespace Models

#endif // MODELS_FILEPROPERTYPROPERTY_HPP
