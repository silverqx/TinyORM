#pragma once
#ifndef TOM_TOMTYPES_HPP
#define TOM_TOMTYPES_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QVector>

#include <map>
#include <memory>
#include <optional>

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
        /*! Migration batch number. */
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

    /*! Migration properties defined on the migration instance (helps to avoid
        the virtual getters). */
    struct MigrationProperties
    {
        /*! Migration name (with the datetime prefix). */
        QString name;
        /*! The name of the database connection to use. */
        QString connection;
        /*! Wrapping the migration within a transaction, if supported. */
        bool withinTransaction = false;
    };

    /*! Subsection item type. */
    struct SubSectionItem
    {
        /*! Subsection item name. */
        std::optional<QString> name;
        /*! About items map. */
        std::map<QString, QString> abouts;
    };

    /*! Section item type. */
    struct SectionItem
    {
        /*! Section item name. */
        QString name;
        /*! Subsection items list. */
        QVector<SubSectionItem> subSections;
    };

} // namespace Tom

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_TOMTYPES_HPP
