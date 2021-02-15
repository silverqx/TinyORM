#ifndef FLIGHT_H
#define FLIGHT_H

#include <orm/tiny/basemodel.hpp>

    class Flight final : public Orm::Tiny::BaseModel<Flight>
    {
    public:
        friend class BaseModel;
        using BaseModel::BaseModel;

    private:
        /*! The database connection that should be used by the model. */
        QString u_connection {"sqlite"};
    };

#endif // FLIGHT_H
