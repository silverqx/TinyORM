#ifndef TAG_H
#define TAG_H

#include "orm/tiny/basemodel.hpp"

#include "models/tagged.hpp"
#include "models/tagproperty.hpp"
#include "models/torrent.hpp"

using Orm::Tiny::BaseModel;
using Orm::Tiny::Relations::BelongsToMany;
using Orm::Tiny::Relations::Pivot;
using Orm::Tiny::Relations::Relation;
using Orm::WithItem;

class Tag final : public Orm::Tiny::BaseModel<Tag, Torrent, TagProperty, Tagged>
//class Tag final : public BaseModel<Tag, Torrent, TagProperty, Pivot>
{
public:
    friend BaseModel;

    using BaseModel::BaseModel;

    /*! Get torrents that belong to the tag. */
    std::unique_ptr<Relation<Tag, Torrent>>
    torrents()
    {
        // Ownership of a unique_ptr()
        auto relation = belongsToMany<Torrent, Pivot>();
        dynamic_cast<BelongsToMany<Tag, Torrent, Pivot> &>(*relation)
//                .as("tagged")
                .withPivot("active")
                .withTimestamps();

        return relation;
    }

    /*! Get the tag property associated with the tag. */
    std::unique_ptr<Relation<Tag, TagProperty>>
    tagProperty()
    {
        return hasOne<TagProperty>();
    }

private:
    /*! The visitor to obtain a type for Related template parameter. */
    void relationVisitor(const QString &relation)
    {
        if (relation      == "torrents")
            relationVisited<Torrent>();
        else if (relation == "tagged") // Pivot
            relationVisited<Tagged>();
        else if (relation == "tagProperty")
            relationVisited<TagProperty>();
    }

    /*! The table associated with the model. */
    QString u_table {"torrent_tags"};

    /*! Map of relation names to methods. */
    QHash<QString, std::any> u_relations {
        {"torrents",    &Tag::torrents},
        {"tagProperty", &Tag::tagProperty},
    };

    /*! The relations to eager load on every query. */
    QVector<WithItem> u_with {
        {"tagProperty"},
    };

#ifdef PROJECT_TINYORM_TEST
    /*! The connection name for the model. */
    QString u_connection {"tinyorm_mysql_tests"};
#endif
};

#endif // TAG_H
