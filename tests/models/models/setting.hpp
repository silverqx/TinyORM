#pragma once
#ifndef MODELS_SETTING_HPP
#define MODELS_SETTING_HPP

#include "orm/tiny/model.hpp"

namespace Models
{

using Orm::Tiny::Model;

class Setting final : public Model<Setting> // NOLINT(bugprone-exception-escape, misc-no-recursion)
{
    friend Model;
    using Model::Model;

    /*! Indicates if the model's ID is auto-incrementing. */
    bool u_incrementing = false;
};

} // namespace Models

#endif // MODELS_SETTING_HPP
