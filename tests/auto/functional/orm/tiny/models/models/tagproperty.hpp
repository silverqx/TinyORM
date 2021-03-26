#ifndef TAGPROPERTY_H
#define TAGPROPERTY_H

#include "orm/tiny/basemodel.hpp"

using Orm::Tiny::BaseModel;

class TagProperty final : public BaseModel<TagProperty>
{
    friend BaseModel;
    using BaseModel::BaseModel;

    /*! The table associated with the model. */
    QString u_table {"tag_properties"};
};

#endif // TAGPROPERTY_H
