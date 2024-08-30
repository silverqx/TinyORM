#pragma once
#ifndef MODELS_TYPE_HPP
#define MODELS_TYPE_HPP

#include "orm/tiny/model.hpp"

namespace Models
{

using Orm::Tiny::Model;

class Type final : public Model<Type> // NOLINT(bugprone-exception-escape, misc-no-recursion)
{
    friend Model;
    using Model::Model;

    /*! Indicates whether the model should be timestamped. */
    bool u_timestamps = false;

    /*! The attributes that should be cast. */
    inline static std::unordered_map<QString, CastItem> u_casts {}; // Don't remove to be defined on Type and not in HasAttributes

public:
    /*! The attributes that should be mutated to dates. */
    inline static QStringList u_dates {};
};

} // namespace Models

#endif // MODELS_TYPE_HPP
