#pragma once
#ifndef MODELS_MASSASSIGNMENTMODELS_HPP
#define MODELS_MASSASSIGNMENTMODELS_HPP

#include "orm/tiny/model.hpp"

namespace Models
{

using Orm::Constants::NAME;
using Orm::Constants::SIZE;

using Orm::Tiny::Model;

// NOLINTNEXTLINE(bugprone-exception-escape)
class Torrent_TotallyGuarded final : public Model<Torrent_TotallyGuarded>
{
    friend Model;
    using Model::Model;

    /*! The table associated with the model. */
    QString u_table {"torrents"};
};

// NOLINTNEXTLINE(bugprone-exception-escape)
class Torrent_GuardedAttribute final : public Model<Torrent_GuardedAttribute>
{
    friend Model;
    using Model::Model;

    /*! The table associated with the model. */
    QString u_table {"torrents"};

    /*! The attributes that are mass assignable. */
    inline static const QStringList u_fillable { // NOLINT(cppcoreguidelines-interfaces-global-init)
        NAME,
        SIZE,
        "progress",
        "added_on",
        "hash",
        "note",
        UPDATED_AT(),
    };

    /*! The attributes that aren't mass assignable. */
    inline static const QStringList u_guarded { // NOLINT(cppcoreguidelines-interfaces-global-init)
        CREATED_AT(),
    };
};

// NOLINTNEXTLINE(bugprone-exception-escape)
class Torrent_AllowedMassAssignment final :
        public Model<Torrent_AllowedMassAssignment>
{
    friend Model;
    using Model::Model;

    /*! The table associated with the model. */
    QString u_table {"torrents"};

    /*! The attributes that aren't mass assignable. */
    inline static QStringList u_guarded {}; // disabled
};

// NOLINTNEXTLINE(bugprone-exception-escape)
class Torrent_GuardableColumn final :
        public Model<Torrent_GuardableColumn>
{
    friend Model;
    using Model::Model;

    /*! The table associated with the model. */
    QString u_table {"torrents"};

    /*! The attributes that aren't mass assignable. */
    inline static QStringList u_guarded {"xyz"};
};

} // namespace Models

#endif // MODELS_MASSASSIGNMENTMODELS_HPP
