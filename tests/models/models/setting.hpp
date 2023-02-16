#pragma once
#ifndef MODELS_SETTING_HPP
#define MODELS_SETTING_HPP

#include "orm/tiny/model.hpp"

namespace Models
{

using Orm::Tiny::Model;

// NOLINTNEXTLINE(bugprone-exception-escape)
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

#endif // MODELS_SETTING_HPP
