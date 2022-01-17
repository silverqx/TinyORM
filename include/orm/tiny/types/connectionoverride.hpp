#pragma once
#ifndef ORM_TINY_TYPES_CONNECTIONOVERRIDE_HPP
#define ORM_TINY_TYPES_CONNECTIONOVERRIDE_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QString>

#include "orm/macros/commonnamespace.hpp"
#include "orm/macros/threadlocal.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny
{
namespace Types
{

    /*! Used by tests to override connection in the Model. */
    struct ConnectionOverride
    {
        /*! The connection to use in the Model, this data member is picked up
            in the Model::getConnectionName(). */
        T_THREAD_LOCAL
        inline static QString connection;

        /*! Deleted default constructor. */
        ConnectionOverride() = delete;
        /*! Deleted destructor. */
        ~ConnectionOverride() = delete;

        Q_DISABLE_COPY(ConnectionOverride)
    };

} // namespace Types

    using ConnectionOverride = Tiny::Types::ConnectionOverride;

} // namespace Orm::Tiny

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_TYPES_CONNECTIONOVERRIDE_HPP
