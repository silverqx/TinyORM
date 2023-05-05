#pragma once
#ifndef MODELS_ALBUM_HPP
#define MODELS_ALBUM_HPP

#include "orm/tiny/model.hpp"

#include "models/albumimage.hpp"

namespace Models
{

using Orm::Constants::NAME;
using Orm::Constants::NOTE;

using Orm::Tiny::Model;
using Orm::Tiny::Relations::HasMany;

class AlbumImage;

// NOLINTNEXTLINE(misc-no-recursion, bugprone-exception-escape)
class Album final : public Model<Album, AlbumImage>
{
    friend Model;
    using Model::Model;

public:
    /*! Get images associated with the album. */
    std::unique_ptr<HasMany<Album, AlbumImage>>
    albumImages()
    {
        return hasMany<AlbumImage>();
    }

private:
    /*! Map of relation names to methods. */
    QHash<QString, RelationVisitor> u_relations {
        {"albumImages", [](auto &v) { v(&Album::albumImages); }},
    };

    /*! The relations to eager load on every query. */
    QVector<QString> u_with {
        "albumImages",
    };

    /*! The attributes that are mass assignable. */
    inline static const QStringList u_fillable { // NOLINT(cppcoreguidelines-interfaces-global-init)
        NAME,
        NOTE,
    };
};

} // namespace Models

#endif // MODELS_ALBUM_HPP
