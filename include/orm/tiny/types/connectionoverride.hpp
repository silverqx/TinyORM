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
        Q_DISABLE_COPY_MOVE(ConnectionOverride)

        /*! Deleted Default constructor. */
        ConnectionOverride() = delete;
        /*! Deleted destructor. */
        ~ConnectionOverride() = delete;

        /*! The connection to use in the Model, this data member is picked up
            in the Model::getConnectionName(). */
        T_THREAD_LOCAL
        inline static QString connection;
    };

} // namespace Types

    /*! Alias for the Types::ConnectionOverride, shortcut alias. */
    using Tiny::Types::ConnectionOverride; // NOLINT(misc-unused-using-decls)

} // namespace Orm::Tiny

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_TYPES_CONNECTIONOVERRIDE_HPP
