#ifndef SETTING_H
#define SETTING_H

#include "orm/tiny/basemodel.hpp"

class Setting final : public Orm::Tiny::BaseModel<Setting>
{
public:
    friend class BaseModel;

    explicit Setting(const QVector<Orm::AttributeItem> &attributes = {});

private:
    /*! The table associated with the model. */
    QString u_table {"settings"};

    /*! Indicates if the model's ID is auto-incrementing. */
    bool u_incrementing = false;

#ifdef PROJECT_TINYORM_TEST
    /*! The connection name for the model. */
    QString u_connection {"tinyorm_mysql_tests"};
#endif
};

#endif // SETTING_H
