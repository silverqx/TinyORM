#pragma once
#ifndef TOM_TOMTYPES_HPP
#define TOM_TOMTYPES_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QString>

#include <memory>

#include <orm/macros/commonnamespace.hpp>

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom
{

    /*! Hydrated migration item from the database. */
    struct MigrationItem
    {
        /*! Database ID. */
        quint64 id;
        /*! Migration name. */
        QString migration;
        int batch;
    };

    class Migration;

    /*! Migration item used and returned from the rollback. */
    struct RollbackItem
    {
        /*! Database ID. */
        quint64 id;
        /*! Migration name. */
        QString migrationName;
        /*! Migration instance. */
        std::shared_ptr<Migration> migration;
    };

} // namespace Tom

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_TOMTYPES_HPP
