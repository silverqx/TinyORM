#pragma once
#ifndef ORM_TINY_RELATIONS_RELATIONTYPES_HPP
#define ORM_TINY_RELATIONS_RELATIONTYPES_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QtGlobal>

#include "orm/macros/commonnamespace.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny::Relations
{

    /*! Tag for one type relation. */
    class OneRelation
    {
        Q_DISABLE_COPY(OneRelation)

    public:
        /*! Default constructor. */
        inline OneRelation() = default;
        /*! Pure virtual destructor. */
        inline virtual ~OneRelation() = 0;
    };

    /*! Tag for many type relation. */
    class ManyRelation
    {
        Q_DISABLE_COPY(ManyRelation)

    public:
        /*! Default constructor. */
        inline ManyRelation() = default;
        /*! Pure virtual destructor. */
        inline virtual ~ManyRelation() = 0;
    };

    /*! Tag for the relation which contains pivot table, like many-to-many. */
    class PivotRelation
    {
        Q_DISABLE_COPY(PivotRelation)

    public:
        /*! Default constructor. */
        inline PivotRelation() = default;
        /*! Pure virtual destructor. */
        inline virtual ~PivotRelation() = 0;
    };

} // namespace Orm::Tiny::Relations

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_RELATIONS_RELATIONTYPES_HPP
