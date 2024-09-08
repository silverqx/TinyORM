#pragma once
#ifndef MODELS_TORRENT_WITHOUT_QDATETIME_HPP
#define MODELS_TORRENT_WITHOUT_QDATETIME_HPP

#include "orm/tiny/model.hpp"

namespace Models
{

using Orm::Constants::HASH_;
using Orm::Constants::NAME;
using Orm::Constants::NOTE;
using Orm::Constants::Progress;
using Orm::Constants::SIZE_;

using Orm::Tiny::Model;

class Torrent_Without_QDateTime final : public Model<Torrent_Without_QDateTime> // NOLINT(bugprone-exception-escape, misc-no-recursion)
{
    friend Model;
    using Model::Model;

    /*! The table associated with the model. */
    QString u_table {u"torrents"_s};

    /*! The model's default values for attributes. */
    inline static const QList<AttributeItem> u_attributes { // NOLINT(cppcoreguidelines-interfaces-global-init)
        {SIZE_,    0},
        {Progress, 0},
    };

    /*! The attributes that are mass assignable. */
    inline static const QStringList u_fillable { // NOLINT(cppcoreguidelines-interfaces-global-init)
        NAME,
        SIZE_,
        Progress,
        NOTE,
    };
};

} // namespace Models

#endif // MODELS_TORRENT_WITHOUT_QDATETIME_HPP
