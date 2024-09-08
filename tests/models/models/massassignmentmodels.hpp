#pragma once
#ifndef MODELS_MASSASSIGNMENTMODELS_HPP
#define MODELS_MASSASSIGNMENTMODELS_HPP

#include "orm/tiny/model.hpp"

namespace Models
{

using Orm::Constants::AddedOn;
using Orm::Constants::HASH_;
using Orm::Constants::NAME;
using Orm::Constants::NOTE;
using Orm::Constants::Progress;
using Orm::Constants::SIZE_;

using Orm::Tiny::Model;

class Torrent_TotallyGuarded final : public Model<Torrent_TotallyGuarded> // NOLINT(bugprone-exception-escape)
{
    friend Model;
    using Model::Model;

    /*! The table associated with the model. */
    QString u_table {u"torrents"_s};
};

class Torrent_GuardedAttribute final : public Model<Torrent_GuardedAttribute> // NOLINT(bugprone-exception-escape)
{
    friend Model;
    using Model::Model;

    /*! The table associated with the model. */
    QString u_table {u"torrents"_s};

    /*! The attributes that are mass assignable. */
    inline static const QStringList u_fillable { // NOLINT(cppcoreguidelines-interfaces-global-init)
        NAME,
        SIZE_,
        Progress,
        AddedOn,
        HASH_,
        NOTE,
        UPDATED_AT(),
    };

    /*! The attributes that aren't mass assignable. */
    inline static const QStringList u_guarded { // NOLINT(cppcoreguidelines-interfaces-global-init)
        CREATED_AT(),
    };
};

class Torrent_AllowedMassAssignment final : public Model<Torrent_AllowedMassAssignment> // NOLINT(bugprone-exception-escape)
{
    friend Model;
    using Model::Model;

    /*! The table associated with the model. */
    QString u_table {u"torrents"_s};

    /*! The attributes that aren't mass assignable. */
    inline static QStringList u_guarded {}; // disabled
};

class Torrent_GuardableColumn final : public Model<Torrent_GuardableColumn> // NOLINT(bugprone-exception-escape)
{
    friend Model;
    using Model::Model;

    /*! The table associated with the model. */
    QString u_table {u"torrents"_s};

    /*! The attributes that aren't mass assignable. */
    inline static QStringList u_guarded {u"xyz"_s};
};

} // namespace Models

#endif // MODELS_MASSASSIGNMENTMODELS_HPP
