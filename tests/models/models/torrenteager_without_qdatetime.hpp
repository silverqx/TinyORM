#pragma once
#ifndef MODELS_TORRENTEAGER_WITHOUT_QDATETIME_HPP
#define MODELS_TORRENTEAGER_WITHOUT_QDATETIME_HPP

#include "orm/tiny/model.hpp"

namespace Models
{

using Orm::Constants::HASH_;
using Orm::Constants::NAME;
using Orm::Constants::NOTE;
using Orm::Constants::Progress;
using Orm::Constants::SIZE_;

using Orm::Tiny::Model;

class TorrentEager_Without_QDateTime final : public Model<TorrentEager_Without_QDateTime> // NOLINT(bugprone-exception-escape, misc-no-recursion)
{
    friend Model;
    using Model::Model;

    /*! The table associated with the model. */
    QString u_table {"torrents"};

    /*! The model's default values for attributes. */
    inline static const QVector<AttributeItem> u_attributes { // NOLINT(cppcoreguidelines-interfaces-global-init)
        {SIZE_,    0},
        {Progress, 0},
    };

    /*! The attributes that are mass assignable. */
    inline static const QStringList u_fillable { // NOLINT(cppcoreguidelines-interfaces-global-init)
        NAME,
        SIZE_,
        Progress,
        "added_on",
        HASH_,
        NOTE,
    };

    /*! The attributes that should be mutated to dates. */
    inline static const QStringList u_dates {"added_on"};
};

} // namespace Models

#endif // MODELS_TORRENTEAGER_WITHOUT_QDATETIME_HPP
