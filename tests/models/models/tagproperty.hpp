#pragma once
#ifndef TAGPROPERTY_HPP
#define TAGPROPERTY_HPP

#include "orm/tiny/model.hpp"

using Orm::Tiny::Model;

class TagProperty final : public Model<TagProperty>
{
    friend Model;
    using Model::Model;

    /*! The table associated with the model. */
    QString u_table {"tag_properties"};
};

#endif // TAGPROPERTY_HPP
