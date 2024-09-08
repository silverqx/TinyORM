#pragma once
#ifndef MODELS_TAGPROPERTY_HPP
#define MODELS_TAGPROPERTY_HPP

#include "orm/tiny/model.hpp"

namespace Models
{

using Orm::Tiny::Model;

class TagProperty final : public Model<TagProperty> // NOLINT(bugprone-exception-escape)
{
    friend Model;
    using Model::Model;

    /*! The table associated with the model. */
    QString u_table {u"tag_properties"_s};
};

} // namespace Models

#endif // MODELS_TAGPROPERTY_HPP
