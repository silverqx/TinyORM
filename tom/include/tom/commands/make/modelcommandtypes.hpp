#pragma once
#ifndef TOM_COMMANDS_MAKE_MODELCOMMANDTYPES_HPP
#define TOM_COMMANDS_MAKE_MODELCOMMANDTYPES_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QStringList>

#include <orm/macros/commonnamespace.hpp>

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands::Make
{

    /*! Foreign key names for the belongs-to-many relation. */
    struct BelongToManyForeignKeys
    {
        /*! The foreign key of the parent model. */
        QString foreignPivotKey;
        /*! The associated key of the relation. */
        QString relatedPivotKey;
    };

    /*! Foreign keys lists divided by relation types. */
    struct ForeignKeys
    {
        /*! Foreign keys for one-to-one relations. */
        QStringList oneToOne {};
        /*! Foreign keys for one-to-many relations. */
        QStringList oneToMany {};
        /*! Foreign keys for belongs-to relations. */
        QStringList belongsTo {};
        /*! Foreign keys for belongs-to-many relations. */
        std::vector<BelongToManyForeignKeys> belongsToMany {};
    };

    /*! Relationship methods order defined on the command-line. */
    struct RelationsOrder
    {
        /*! Oder of one-to-one relations. */
        std::vector<std::size_t> oneToOne {};
        /*! Oder of one-to-many relations. */
        std::vector<std::size_t> oneToMany {};
        /*! Oder of belongs-to relations. */
        std::vector<std::size_t> belongsTo {};
        /*! Oder of belongs-to-many relations. */
        std::vector<std::size_t> belongsToMany {};
    };

    /*! Struct to hold command-line option values. */
    struct CmdOptions
    {
        /* Relationship methods */
        /*! Relationship methods order defined on the command-line. */
        RelationsOrder relationsOrder;
        /*! Related class names for the one-to-one relationship. */
        QStringList oneToOneList;
        /*! Related class names for the one-to-many relationship. */
        QStringList oneToManyList;
        /*! Related class names for the belongs-to relationship. */
        QStringList belongsToList;
        /*! Related class name for the belongs-to-many relationship. */
        QStringList belongsToManyList;
        /* Common for all relationship methods */
        /*! The foreign key names list, every relation can have one foreign key. */
        ForeignKeys foreignKeys;
        /* Belongs-to-many related */
        /*! Pivot table names list. */
        QStringList pivotTables;
        /*! Pivot class names list for the belongs-to-many relationships. */
        QStringList pivotClasses;
        /*! Pivot class names list for the belongs-to-many inverse relationships. */
        std::vector<QStringList> pivotInverseClasses;
        /*! Pivot relation names list. */
        QStringList asList;
        /*! Extra attributes for the pivot model. */
        std::vector<QStringList> withPivotList;
        /*! Pivot table with timestamps. */
        std::vector<bool> withTimestampsList;
        /* Model related */
        /*! The connection name for the model. */
        QString connection;
        /*! The table associated with the model. */
        QString table;
        /*! Disable timestamping of the model. */
        bool disableTimestamps;
        /*! Enable incrmenteing of model's primary key. */
        bool incrementing;
        /*! Genarate a custom pivot model class. */
        bool pivotModel;
    };

} // namespace Tom::Commands::Make

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_MAKE_MODELCOMMANDTYPES_HPP
