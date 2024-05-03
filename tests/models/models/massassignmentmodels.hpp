#pragma once
#ifndef MODELS_MASSASSIGNMENTMODELS_HPP
#define MODELS_MASSASSIGNMENTMODELS_HPP

#include "orm/tiny/model.hpp"

namespace Models
{

using Orm::Constants::HASH_;
using Orm::Constants::NAME;
using Orm::Constants::NOTE;
using Orm::Constants::Progress;
using Orm::Constants::SIZE_;

using Orm::Tiny::Model;

class Torrent_TotallyGuarded final : public Model<Torrent_TotallyGuarded> // NOLINT(bugprone-exception-escape, misc-no-recursion)
{
    friend Model;
    using Model::Model;

    /*! The table associated with the model. */
    QString u_table {"torrents"};
};

class Torrent_GuardedAttribute final : public Model<Torrent_GuardedAttribute> // NOLINT(bugprone-exception-escape, misc-no-recursion)
{
    friend Model;
    using Model::Model;

    /*! The table associated with the model. */
    QString u_table {"torrents"};

    /*! The attributes that are mass assignable. */
    inline static const QStringList u_fillable { // NOLINT(cppcoreguidelines-interfaces-global-init)
        NAME,
        SIZE_,
        Progress,
        "added_on",
        HASH_,
        NOTE,
        UPDATED_AT(),
    };

    /*! The attributes that aren't mass assignable. */
    inline static const QStringList u_guarded { // NOLINT(cppcoreguidelines-interfaces-global-init)
        CREATED_AT(),
    };
};

class Torrent_AllowedMassAssignment final : // NOLINT(bugprone-exception-escape, misc-no-recursion)
        public Model<Torrent_AllowedMassAssignment>
{
    friend Model;
    using Model::Model;

    /*! The table associated with the model. */
    QString u_table {"torrents"};

    /*! The attributes that aren't mass assignable. */
    inline static QStringList u_guarded {}; // disabled
};

class Torrent_GuardableColumn final : // NOLINT(bugprone-exception-escape, misc-no-recursion)
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
