#pragma once
#ifndef MODELS_FILEPROPERTYPROPERTY_HPP
#define MODELS_FILEPROPERTYPROPERTY_HPP

#include "orm/tiny/model.hpp"

namespace Models
{

using Orm::Tiny::Model;

class TorrentPreviewableFileProperty;

class FilePropertyProperty final : // NOLINT(bugprone-exception-escape, misc-no-recursion)
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
                    {}, {}, QString::fromUtf8(__func__)); // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    }

private:
    /*! The table associated with the model. */
    QString u_table {u"file_property_properties"_s};

    /*! Map of relation names to methods. */
    QHash<QString, RelationVisitor> u_relations {
        {u"fileProperty"_s, [](auto &v) { v(&FilePropertyProperty::fileProperty); }},
    };

    /*! All of the relationships to be touched. */
    QStringList u_touches {u"fileProperty"_s};
};

} // namespace Models

#endif // MODELS_FILEPROPERTYPROPERTY_HPP
