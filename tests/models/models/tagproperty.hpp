#pragma once
#ifndef MODELS_TAGPROPERTY_HPP
#define MODELS_TAGPROPERTY_HPP

#include "orm/tiny/model.hpp"

namespace Models
{

using Orm::Tiny::Model;

class TagProperty final : public Model<TagProperty>
{
    friend Model;
    using Model::Model;

    /*! The table associated with the model. */
    QString u_table {"tag_properties"};
};

} // namespace Models

#endif // MODELS_TAGPROPERTY_HPP
