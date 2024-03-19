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
        IsRelation(const IsRelation &) = default;

    public:
        /*! Default constructor. */
        IsRelation() = default;
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
    class IsOneRelation
    {
        Q_DISABLE_COPY_MOVE(IsOneRelation)

    public:
        /*! Default constructor. */
        IsOneRelation() = default;
        /*! Pure virtual destructor. */
        inline virtual ~IsOneRelation() = 0;
    };

    IsOneRelation::~IsOneRelation() = default;

    /*! Tag for many type relation. */
    class IsManyRelation
    {
    protected:
        /*! IsManyRelation's copy constructor (used by BelongsToMany::clone()). */
        IsManyRelation(const IsManyRelation &) = default;

    public:
        /*! Default constructor. */
        IsManyRelation() = default;
        /*! Pure virtual destructor. */
        inline virtual ~IsManyRelation() = 0;

        /*! IsManyRelation's move constructor. */
        IsManyRelation(IsManyRelation &&) = delete;

        /*! IsManyRelation's copy assignment operator. */
        IsManyRelation &operator=(const IsManyRelation &) = delete;
        /*! IsManyRelation's move assignment operator. */
        IsManyRelation &operator=(IsManyRelation &&) = delete;
    };

    IsManyRelation::~IsManyRelation() = default;

    /*! Tag for the relation which contains pivot table, like many-to-many. */
    class IsPivotRelation
    {
    protected:
        /*! IsPivotRelation's copy constructor (used by BelongsToMany::clone()). */
        IsPivotRelation(const IsPivotRelation &) = default;

    public:
        /*! Default constructor. */
        IsPivotRelation() = default;
        /*! Pure virtual destructor. */
        inline virtual ~IsPivotRelation() = 0;

        /*! IsPivotRelation's move constructor. */
        IsPivotRelation(IsPivotRelation &&) = delete;

        /*! IsPivotRelation's copy assignment operator. */
        IsPivotRelation &operator=(const IsPivotRelation &) = delete;
        /*! IsPivotRelation's move assignment operator. */
        IsPivotRelation &operator=(IsPivotRelation &&) = delete;
    };

    IsPivotRelation::~IsPivotRelation() = default;

} // namespace Orm::Tiny::Relations

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_RELATIONS_RELATIONTYPES_HPP
