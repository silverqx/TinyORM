#ifndef FILEPROPERTYPROPERTY_H
#define FILEPROPERTYPROPERTY_H

#include "orm/tiny/basemodel.hpp"

#include "models/torrentpreviewablefileproperty.hpp"

class FilePropertyProperty final :
        public Orm::Tiny::BaseModel<FilePropertyProperty, TorrentPreviewableFileProperty>
{
    friend BaseModel;
    using BaseModel::BaseModel;

public:
    /*! Get the previewable file that owns the file property. */
    std::unique_ptr<
    Orm::Tiny::Relations::Relation<FilePropertyProperty, TorrentPreviewableFileProperty>>
    fileProperty()
    {
        return belongsTo<TorrentPreviewableFileProperty>("file_property_id", {},
                                                         __func__);
    }

private:
    /*! The visitor to obtain a type for Related template parameter. */
    void relationVisitor(const QString &relation)
    {
        if (relation == "fileProperty")
            relationVisited<TorrentPreviewableFileProperty>();
    }

    /*! The table associated with the model. */
    QString u_table {"file_property_properties"};

    /*! Map of relation names to methods. */
    QHash<QString, std::any> u_relations {
        {"fileProperty", &FilePropertyProperty::fileProperty},
    };

    /*! The relations to eager load on every query. */
    QVector<Orm::WithItem> u_with {
//        {"fileProperty"},
    };

#ifdef PROJECT_TINYORM_TEST
    /*! The connection name for the model. */
    QString u_connection {"tinyorm_mysql_tests"};
#endif

    /*! All of the relationships to be touched. */
    QStringList u_touches {"fileProperty"};
};

#endif // FILEPROPERTYPROPERTY_H
