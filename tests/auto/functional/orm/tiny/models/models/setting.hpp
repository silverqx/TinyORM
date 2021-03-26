#ifndef SETTING_H
#define SETTING_H

#include "orm/tiny/basemodel.hpp"

using Orm::Tiny::BaseModel;

class Setting final : public BaseModel<Setting>
{
    friend BaseModel;
    using BaseModel::BaseModel;

private:
    /*! The table associated with the model. */
    QString u_table {"settings"};

    /*! Indicates if the model's ID is auto-incrementing. */
    bool u_incrementing = false;
};

#endif // SETTING_H
