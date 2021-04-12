#ifndef TAGPROPERTY_H
#define TAGPROPERTY_H

#include "orm/tiny/model.hpp"

using Orm::Tiny::Model;

class TagProperty final : public Model<TagProperty>
{
    friend Model;
    using Model::Model;

    /*! The table associated with the model. */
    QString u_table {"tag_properties"};
};

#endif // TAGPROPERTY_H
