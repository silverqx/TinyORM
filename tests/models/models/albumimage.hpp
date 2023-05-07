#pragma once
#ifndef MODELS_ALBUMIMAGE_HPP
#define MODELS_ALBUMIMAGE_HPP

#include "orm/tiny/model.hpp"

#include "models/album.hpp"

namespace Models
{

using Orm::Constants::ID;
using Orm::Constants::NAME;
using Orm::Constants::SIZE_;

using Orm::Tiny::Model;
using Orm::Tiny::Relations::BelongsTo;

class Album;

// NOLINTNEXTLINE(misc-no-recursion, bugprone-exception-escape)
class AlbumImage final : public Model<AlbumImage, Album>
{
    friend Model;
    using Model::Model;

public:
    /*! Get an album associated with the image. */
    std::unique_ptr<BelongsTo<AlbumImage, Album>>
    album()
    {
        return belongsTo<Album>();
    }

private:
    /*! Map of relation names to methods. */
    QHash<QString, RelationVisitor> u_relations {
        {"album", [](auto &v) { v(&AlbumImage::album); }},
    };

    /*! The relations to eager load on every query. */
//    QVector<QString> u_with {
//        "album",
//    };

    /*! The attributes that are mass assignable. */
    inline static QStringList u_fillable { // NOLINT(cppcoreguidelines-interfaces-global-init)
        ID,
        NAME,
        "ext",
        SIZE_,
    };
};

} // namespace Models

#endif // MODELS_ALBUMIMAGE_HPP
