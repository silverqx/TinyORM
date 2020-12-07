#ifndef SETTING_H
#define SETTING_H

#include "orm/tiny/basemodel.h"

class Setting final : public Orm::Tiny::BaseModel<Setting>
{
public:
    friend class BaseModel;

    explicit Setting(const QVector<Orm::AttributeItem> &attributes = {});

private:
    // TODO next test no relation behavior silverqx
//    void eagerVisitor(const QString &)
//    {}

    /*! The table associated with the model. */
    QString u_table {"settings"};

    /*! Indicates if the IDs are auto-incrementing. */
    bool u_incrementing = false;
};

#endif // SETTING_H
