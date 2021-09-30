#pragma once
#ifndef TAG_HPP
#define TAG_HPP

#include "orm/tiny/model.hpp"

#include "models/tagged.hpp"
#include "models/tagproperty.hpp"

using namespace Orm::Constants;

using Orm::Tiny::Model;
using Orm::Tiny::Relations::BelongsToMany;
using Orm::Tiny::Relations::HasOne;
using Orm::Tiny::Relations::Pivot;

class Torrent;

class Tag final : public Model<Tag, Torrent, TagProperty, Tagged>
//class Tag final : public Model<Tag, Torrent, TagProperty, Pivot>
{
    friend Model;
    using Model::Model;

public:
    /*! Get torrents that belong to the tag. */
    std::unique_ptr<BelongsToMany<Tag, Torrent, Pivot>>
    torrents()
    {
        // Ownership of a unique_ptr()
        auto relation = belongsToMany<Torrent, Pivot>();

        relation/*->as("tagged")*/
                ->withPivot("active")
                .withTimestamps();

        return relation;
    }

    /*! Get torrents that belong to the tag. */
    std::unique_ptr<BelongsToMany<Tag, Torrent, Pivot>>
    torrents_WithoutPivotAttributes()
    {
        // Ownership of a unique_ptr()
        return belongsToMany<Torrent, Pivot>();
    }

    /*! Get a tag property associated with the tag. */
    std::unique_ptr<HasOne<Tag, TagProperty>>
    tagProperty()
    {
        return hasOne<TagProperty>();
    }

private:
    /*! The table associated with the model. */
    QString u_table {"torrent_tags"};

    /*! Map of relation names to methods. */
    QHash<QString, RelationVisitor> u_relations {
        {"torrents",                        [](auto &v) { v(&Tag::torrents); }},
        {"torrents_WithoutPivotAttributes", [](auto &v) { v(&Tag::torrents_WithoutPivotAttributes); }},
        {"tagProperty",                     [](auto &v) { v(&Tag::tagProperty); }},
    };

    /*! The relations to eager load on every query. */
    QVector<QString> u_with {
        "tagProperty",
    };

    /*! The attributes that are mass assignable. */
    inline static QStringList u_fillable {
        NAME,
        "note",
    };

    /*! All of the relationships to be touched. */
    QStringList u_touches {"torrents"};
};

#endif // TAG_HPP
