#pragma once
#ifndef MODELS_TORRENTEAGER_WITHOUT_QDATETIME_HPP
#define MODELS_TORRENTEAGER_WITHOUT_QDATETIME_HPP

#include "orm/tiny/model.hpp"

namespace Models
{

using Orm::Constants::NAME;
using Orm::Constants::SIZE;

using Orm::Tiny::AttributeItem;
using Orm::Tiny::Model;

class TorrentEager_Without_QDateTime final :
        public Model<TorrentEager_Without_QDateTime>
{
    friend Model;
    using Model::Model;

    /*! The table associated with the model. */
    QString u_table {"torrents"};

    /*! The model's default values for attributes. */
    inline static const QVector<AttributeItem> u_attributes { // NOLINT(cppcoreguidelines-interfaces-global-init)
        {SIZE,       0},
        {"progress", 0},
    };

    /*! The attributes that are mass assignable. */
    inline static const QStringList u_fillable { // NOLINT(cppcoreguidelines-interfaces-global-init)
        NAME,
        SIZE,
        "progress",
        "added_on",
        "hash",
        "note",
    };

    /*! The attributes that should be mutated to dates. */
    inline static const QStringList u_dates {"added_on"};
};

} // namespace Models

#endif // MODELS_TORRENTEAGER_WITHOUT_QDATETIME_HPP
