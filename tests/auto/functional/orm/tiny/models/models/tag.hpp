#ifndef TAG_H
#define TAG_H

#include "orm/tiny/model.hpp"

#include "models/tagged.hpp"
#include "models/tagproperty.hpp"
#include "models/torrent.hpp"

using Orm::Tiny::Model;
using Orm::Tiny::Relations::BelongsToMany;
using Orm::Tiny::Relations::Pivot;
using Orm::Tiny::Relations::Relation;
using Orm::WithItem;

class Tag final : public Model<Tag, Torrent, TagProperty, Tagged>
//class Tag final : public Model<Tag, Torrent, TagProperty, Pivot>
{
    friend Model;
    using Model::Model;

public:
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

    /*! The attributes that are mass assignable. */
    inline static QStringList u_fillable {
        "name",
    };

    /*! All of the relationships to be touched. */
    QStringList u_touches {"torrents"};
};

#endif // TAG_H
