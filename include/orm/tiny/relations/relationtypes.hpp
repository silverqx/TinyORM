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

    /*! Tag for the relation type. */
    class IsRelation
    {
    protected:
        /*! IsRelation's copy constructor (used by BelongsToMany::clone()). */
        inline IsRelation(const IsRelation &) = default;

    public:
        /*! Default constructor. */
        inline IsRelation() = default;
        /*! Pure virtual destructor. */
        inline virtual ~IsRelation() = 0;

        /*! IsRelation's move constructor. */
        IsRelation(IsRelation &&) = delete;

        /*! IsRelation's copy assignment operator. */
        IsRelation &operator=(const IsRelation &) = delete;
        /*! IsRelation's move assignment operator. */
        IsRelation &operator=(IsRelation &&) = delete;
    };

    IsRelation::~IsRelation() = default;

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

    OneRelation::~OneRelation() = default;

    /*! Tag for many type relation. */
    class ManyRelation
    {
    protected:
        /*! ManyRelation's copy constructor (used by BelongsToMany::clone()). */
        inline ManyRelation(const ManyRelation &) = default;

    public:
        /*! Default constructor. */
        inline ManyRelation() = default;
        /*! Pure virtual destructor. */
        inline virtual ~ManyRelation() = 0;

        /*! ManyRelation's move constructor. */
        ManyRelation(ManyRelation &&) = delete;

        /*! ManyRelation's copy assignment operator. */
        ManyRelation &operator=(const ManyRelation &) = delete;
        /*! ManyRelation's move assignment operator. */
        ManyRelation &operator=(ManyRelation &&) = delete;
    };

    ManyRelation::~ManyRelation() = default;

    /*! Tag for the relation which contains pivot table, like many-to-many. */
    class PivotRelation
    {
    protected:
        /*! PivotRelation's copy constructor (used by BelongsToMany::clone()). */
        inline PivotRelation(const PivotRelation &) = default;

    public:
        /*! Default constructor. */
        inline PivotRelation() = default;
        /*! Pure virtual destructor. */
        inline virtual ~PivotRelation() = 0;

        /*! PivotRelation's move constructor. */
        PivotRelation(PivotRelation &&) = delete;

        /*! PivotRelation's copy assignment operator. */
        PivotRelation &operator=(const PivotRelation &) = delete;
        /*! PivotRelation's move assignment operator. */
        PivotRelation &operator=(PivotRelation &&) = delete;
    };

    PivotRelation::~PivotRelation() = default;

} // namespace Orm::Tiny::Relations

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_RELATIONS_RELATIONTYPES_HPP
