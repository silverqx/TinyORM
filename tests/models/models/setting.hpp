#pragma once
#ifndef SETTING_HPP
#define SETTING_HPP

#include "orm/tiny/model.hpp"

namespace Models
{

using Orm::Tiny::Model;

class Setting final : public Model<Setting>
{
    friend Model;
    using Model::Model;

private:
    /*! The table associated with the model. */
    QString u_table {"settings"};

    /*! Indicates if the model's ID is auto-incrementing. */
    bool u_incrementing = false;
};

} // namespace Models

#endif // SETTING_HPP
