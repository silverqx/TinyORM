#pragma once
#ifndef TAGPROPERTY_HPP
#define TAGPROPERTY_HPP

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

#endif // TAGPROPERTY_HPP
