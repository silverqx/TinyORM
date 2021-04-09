#ifndef MASSASSIGNMENTMODELS_HPP
#define MASSASSIGNMENTMODELS_HPP

#include "orm/tiny/basemodel.hpp"

using Orm::Tiny::BaseModel;

class Torrent_TotallyGuarded final : public BaseModel<Torrent_TotallyGuarded>
{
    friend BaseModel;
    using BaseModel::BaseModel;

    /*! The table associated with the model. */
    QString u_table {"torrents"};
};

class Torrent_GuardedAttribute final : public BaseModel<Torrent_GuardedAttribute>
{
    friend BaseModel;
    using BaseModel::BaseModel;

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
        public BaseModel<Torrent_AllowedMassAssignment>
{
    friend BaseModel;
    using BaseModel::BaseModel;

    /*! The table associated with the model. */
    QString u_table {"torrents"};

    /*! The attributes that aren't mass assignable. */
    inline static QStringList u_guarded {}; // disabled
};

#endif // MASSASSIGNMENTMODELS_HPP
