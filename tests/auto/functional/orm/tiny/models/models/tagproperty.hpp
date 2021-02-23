#ifndef TAGPROPERTY_H
#define TAGPROPERTY_H

#include "orm/tiny/basemodel.hpp"

using Orm::Tiny::BaseModel;

class TagProperty final : public BaseModel<TagProperty>
{
public:
    friend BaseModel;

    using BaseModel::BaseModel;

private:
    /*! The table associated with the model. */
    QString u_table {"tag_properties"};

#ifdef PROJECT_TINYORM_TEST
    /*! The connection name for the model. */
    QString u_connection {"tinyorm_mysql_tests"};
#endif
};

#endif // TAGPROPERTY_H
