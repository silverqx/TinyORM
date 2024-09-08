#pragma once
#ifndef MODELS_TAG_HPP
#define MODELS_TAG_HPP

#include "orm/tiny/model.hpp"

namespace Models
{

using Orm::Constants::NAME;
using Orm::Constants::NOTE;

using Orm::Tiny::Model;
//using Orm::Tiny::Relations::Pivot;

class Role;
class TagProperty;
class Tagged;
class Torrent;

class Tag final : public Model<Tag, Torrent, TagProperty, Role, Tagged> // NOLINT(bugprone-exception-escape, misc-no-recursion)
//class Tag final : public Model<Tag, Torrent, TagProperty, Role, Pivot> // NOLINT(bugprone-exception-escape, misc-no-recursion)
{
    friend Model;
    using Model::Model;

public:
    /*! Get torrents that belong to the tag. */
    std::unique_ptr<BelongsToMany<Tag, Torrent>>
    torrents()
    {
        // Ownership of a unique_ptr()
        auto relation = belongsToMany<Torrent>();

        relation/*->as(u"tagged"_s)*/
                ->withPivot(u"active"_s)
                 .withTimestamps();

        return relation;
    }

    /*! Get torrents that belong to the tag. */
    std::unique_ptr<BelongsToMany<Tag, Torrent>>
    torrents_WithoutPivotAttributes()
    {
        // Ownership of a unique_ptr()
        return belongsToMany<Torrent>({}, {}, {}, {}, {},
                                      u"torrents_WithoutPivotAttributes"_s);
    }

    /*! Get a tag property associated with the tag. */
    std::unique_ptr<HasOne<Tag, TagProperty>>
    tagProperty()
    {
        return hasOne<TagProperty>();
    }

    /*! The roles that belong to the tag. */
    std::unique_ptr<BelongsToMany<Tag, Role>>
    roles()
    {
        // Ownership of a unique_ptr()
        auto relation = belongsToMany<Role>();

        relation->as(u"acl"_s)
                 .withPivot(u"active"_s);

        return relation;
    }

private:
    /*! The table associated with the model. */
    QString u_table {u"torrent_tags"_s};

    /*! Map of relation names to methods. */
    QHash<QString, RelationVisitor> u_relations {
        {u"torrents"_s,                        [](auto &v) { v(&Tag::torrents); }},
        {u"torrents_WithoutPivotAttributes"_s, [](auto &v) { v(&Tag::torrents_WithoutPivotAttributes); }},
        {u"tagProperty"_s,                     [](auto &v) { v(&Tag::tagProperty); }},
        {u"roles"_s,                           [](auto &v) { v(&Tag::roles); }},
    };

    /*! The relations to eager load on every query. */
    QList<QString> u_with {
        u"tagProperty"_s,
    };

    /*! The attributes that are mass assignable. */
    inline static const QStringList u_fillable { // NOLINT(cppcoreguidelines-interfaces-global-init)
        NAME,
        NOTE,
    };

    /*! All of the relationships to be touched. */
    QStringList u_touches {u"torrents"_s};
};

} // namespace Models

#endif // MODELS_TAG_HPP
