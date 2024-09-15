#pragma once
#ifndef ORM_DRIVERS_SUPPORT_SQLRECORDCACHE_P_HPP
#define ORM_DRIVERS_SUPPORT_SQLRECORDCACHE_P_HPP

#include "orm/drivers/sqlrecord.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers::Support
{

    /*! Cache for the record/WithDefaultValues() methods (SqlRecord). */
    struct SqlRecordCachePrivate
    {
        /*! Cached SqlRecord instance. */
        std::optional<SqlRecord> cache = std::nullopt;
        /*! Determine whether the cached SqlRecord contains Default Column Values. */
        bool hasDefaultValues = false;

        /*! Determine if a SqlRecord is cached. */
        constexpr bool has() const noexcept;
        /*! Determine if a SqlRecord is cached. */
        constexpr explicit operator bool() const noexcept; // explicit is always desirable with the operator bool() to avoid unintentional conversions

        /*! Retrieve a SqlRecord from the cache. */
        constexpr SqlRecord &get();
        /*! Retrieve a SqlRecord from the cache, const version. */
        constexpr const SqlRecord &get() const;

        /*! Retrieve a SqlRecord from the cache. */
        constexpr SqlRecord &operator*() noexcept;
        /*! Retrieve a SqlRecord from the cache, const version. */
        constexpr const SqlRecord &operator*() const noexcept;

        /*! Store the given SqlRecord in the cache. */
        constexpr SqlRecord &put(SqlRecord &&record, bool defaultValues) noexcept;
        /*! Store the given SqlRecord in the cache (helper method only). */
        constexpr SqlRecord &put(SqlRecord &record, bool defaultValues) noexcept;

        /*! Clear the cache. */
        constexpr void clear() noexcept;
    };

    /* public */

    constexpr bool SqlRecordCachePrivate::has() const noexcept
    {
        return cache.has_value();
    }

    constexpr SqlRecordCachePrivate::operator bool() const noexcept
    {
        return cache.has_value();
    }

    constexpr SqlRecord &SqlRecordCachePrivate::get()
    {
        return cache.value(); // NOLINT(bugprone-unchecked-optional-access)
    }

    constexpr const SqlRecord &SqlRecordCachePrivate::get() const
    {
        return cache.value(); // NOLINT(bugprone-unchecked-optional-access)
    }

    constexpr SqlRecord &SqlRecordCachePrivate::operator*() noexcept
    {
        return *cache; // NOLINT(bugprone-unchecked-optional-access)
    }

    constexpr const SqlRecord &SqlRecordCachePrivate::operator*() const noexcept
    {
        return *cache; // NOLINT(bugprone-unchecked-optional-access)
    }

    constexpr SqlRecord &
    SqlRecordCachePrivate::put(SqlRecord &&record, const bool defaultValues) noexcept
    {
        cache = std::move(record);
        hasDefaultValues = defaultValues;

        return *cache;
    }

    constexpr SqlRecord &
    SqlRecordCachePrivate::put(SqlRecord &record, const bool defaultValues) noexcept
    {
        /* This is only a helper method to have a nicer code, it allows to call the return
           statement like: return cache.put(SqlRecord &). In this case the given SqlRecord
           must have the same memory address as the current cached SqlRecord.
           Used with: populateFieldDefaultValues(d->recordCache.get()) */
        Q_ASSERT(cache && std::addressof(record) == std::addressof(*cache));

        hasDefaultValues = defaultValues;

        return record;
    }

    constexpr void SqlRecordCachePrivate::clear() noexcept
    {
        cache.reset();
        hasDefaultValues = false;
    }

} // namespace Orm::Drivers::Support

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_SUPPORT_SQLRECORDCACHE_P_HPP
