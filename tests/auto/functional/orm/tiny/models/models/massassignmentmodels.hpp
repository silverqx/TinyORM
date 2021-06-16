#pragma once
#ifndef MASSASSIGNMENTMODELS_HPP
#define MASSASSIGNMENTMODELS_HPP

#include "orm/tiny/model.hpp"

using Orm::Tiny::Model;

class Torrent_TotallyGuarded final : public Model<Torrent_TotallyGuarded>
{
    friend Model;
    using Model::Model;

    /*! The table associated with the model. */
    QString u_table {"torrents"};
};

class Torrent_GuardedAttribute final : public Model<Torrent_GuardedAttribute>
{
    friend Model;
    using Model::Model;

    /*! The table associated with the model. */
    QString u_table {"torrents"};

    /*! The attributes that are mass assignable. */
    inline static QStringList u_fillable {
        "name",
        "size",
        "progress",
        "added_on",
        "hash",
        "note",
        "updated_at",
    };

    /*! The attributes that aren't mass assignable. */
    inline static QStringList u_guarded {
        "created_at",
    };
};

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

#endif // MASSASSIGNMENTMODELS_HPP
