#pragma once
#ifndef ORM_TINY_TYPES_SYNCCHANGES_HPP
#define ORM_TINY_TYPES_SYNCCHANGES_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QList>
#include <QVariant>

#include <map>

#if defined(__clang__) || (defined(_MSC_VER) && _MSC_VER < 1929)
#  include <range/v3/algorithm/unique.hpp>
#endif

#include "orm/macros/commonnamespace.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{

namespace Constants
{
    inline const auto Attached = QStringLiteral("attached");
    inline const auto Detached = QStringLiteral("detached");
    inline const auto Updated_ = QStringLiteral("updated");
} // namespace Constants

namespace Tiny
{
namespace Types
{

    /*! Result of the sync() related methods in belongs-to-many relation type. */
    class SyncChanges
    {
    public:
        /* Container related */
        using key_type               = QString;
        using mapped_type            = QList<QVariant>;
        using value_type             = typename std::pair<const key_type, mapped_type>;
        using reference              = value_type &;
        using const_reference        = const value_type &;
        using ContainerType          = typename std::map<key_type, mapped_type>;
        /* Iterators related */
        using iterator               = typename ContainerType::iterator;
        using const_iterator         = typename ContainerType::const_iterator;
        using reverse_iterator       = typename ContainerType::reverse_iterator;
        using const_reverse_iterator = typename ContainerType::const_reverse_iterator;
        using difference_type        = typename ContainerType::difference_type;
        using size_type              = typename ContainerType::size_type;
        using mapped_size_type       = typename mapped_type::size_type;

        /*! Default constructor. */
        inline SyncChanges();
        /*! Default destructor. */
        ~SyncChanges() = default;

        /*! Copy constructor. */
        SyncChanges(const SyncChanges &) = default;
        /*! Copy assignment operator. */
        SyncChanges &operator=(const SyncChanges &) = default;

        /*! Move constructor. */
        SyncChanges(SyncChanges &&) // NOLINT(bugprone-exception-escape)
        noexcept(std::is_nothrow_move_constructible_v<ContainerType>) = default; // NOLINT(performance-noexcept-move-constructor)
        /*! Move assignment operator. */
        SyncChanges &operator=(SyncChanges &&) // NOLINT(bugprone-exception-escape)
        noexcept(std::is_nothrow_move_assignable_v<ContainerType>) = default; // NOLINT(performance-noexcept-move-constructor)

        /* SyncChanges related */
        /*! Merge changes into the current instance. */
        template<typename KeyType>
        SyncChanges &merge(SyncChanges &&changes);

        /*! Determine if the given key is supported. */
        inline bool isValidKey(const QString &key) const;

        /* Getters */
        /*! Vector of attached models IDs. */
        inline QList<QVariant> &attached();
        /*! Vector of attached models IDs, const version. */
        inline const QList<QVariant> &attached() const;
        /*! Vector of detached models IDs. */
        inline QList<QVariant> &detached();
        /*! Vector of detached models IDs, const version. */
        inline const QList<QVariant> &detached() const;
        /*! Vector of model IDs for which pivot records were updated in the pivot
            table. */
        inline QList<QVariant> &updated();
        /*! Vector of model IDs for which pivot records were updated in the pivot table,
            const version. */
        inline const QList<QVariant> &updated() const;

        /* std::map proxy methods */
        /*! Returns an iterator to the beginning. */
        inline iterator begin() noexcept;
        /*! Returns an iterator to the end. */
        inline iterator end() noexcept;
        /*! Returns an iterator to the beginning, const version. */
        inline const_iterator begin() const noexcept;
        /*! Returns an iterator to the end, const version. */
        inline const_iterator end() const noexcept;
        /*! Returns an iterator to the beginning, const version. */
        inline const_iterator cbegin() const noexcept;
        /*! Returns an iterator to the end, const version. */
        inline const_iterator cend() const noexcept;

        /*! Returns a reverse iterator to the beginning. */
        inline reverse_iterator rbegin() noexcept;
        /*! Returns a reverse iterator to the end. */
        inline reverse_iterator rend() noexcept;
        /*! Returns a reverse iterator to the beginning, const version. */
        inline const_reverse_iterator rbegin() const noexcept;
        /*! Returns a reverse iterator to the end, const version. */
        inline const_reverse_iterator rend() const noexcept;
        /*! Returns a reverse iterator to the beginning, const version. */
        inline const_reverse_iterator crbegin() const noexcept;
        /*! Returns a reverse iterator to the end, const version. */
        inline const_reverse_iterator crend() const noexcept;

        /* Capacity */
        /*! Returns the number of all elements (attached, detached, updated). */
        inline size_type size() const
        noexcept(std::is_nothrow_invocable_v<decltype (&mapped_type::size),
                                             ContainerType &>);
        /*! Checks whether the container is empty (all attached, detached, updated). */
        inline bool empty() const
        noexcept(std::is_nothrow_invocable_r_v<bool, decltype (&mapped_type::empty),
                                               ContainerType &>);

        /* Element access */
        /*! Access specified element with bounds checking. */
        inline mapped_type &at(const key_type &key);
        /*! Access specified element with bounds checking. */
        inline const mapped_type &at(const key_type &key) const;
        /*! Access specified element with bounds checking, alias to the at(). */
        inline mapped_type &operator[](const key_type &key);
        /*! Access specified element with bounds checking, alias to the at(). */
        inline const mapped_type &operator[](const key_type &key) const;

        /* Modifiers */
        /*! Clears the contents. */
        inline void clear();

        /*! Swap the SyncChanges. */
        inline void swap(SyncChanges &right)
        noexcept(std::is_nothrow_swappable_v<ContainerType>);

        /* Comparison */
        /*! Equality comparison operator for the SyncChanges. */
        bool operator==(const SyncChanges &) const = default;

    protected:
        /*! Cast the given key to the primary key type. */
        template<typename T>
        inline T castKey(const QVariant &key) const;

        /*! SyncChanges storage. */
        ContainerType m_data;
    };

    /* public */

    SyncChanges::SyncChanges()
        : m_data {{Constants::Attached, {}},
                  {Constants::Detached, {}},
                  {Constants::Updated_, {}}}
    {}

    /* SyncChanges related */

    template<typename KeyType>
    SyncChanges &SyncChanges::merge(SyncChanges &&changes) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
    {
        /* I can't find a better place for this check, an alternative is to add
           the dataSize() -> m_data.size() public method and wrap it in
           the ifdef TINYORM_TESTS_CODE, but I don't like this approach as it breaks
           API compatibility. */

        Q_ASSERT(m_data.size() == 3);

        for (auto &&[key, values] : changes) {

            auto &currentValues = at(key);

            {
                // If the current key value is empty, then simply move a new values
                if (isValidKey(key) && currentValues.isEmpty()) {
                    if (!values.isEmpty())
                        currentValues = std::move(values);

                    continue;
                }
            }

            // Otherwise merge values
            const auto castKey = [this](const auto &id)
            {
                return this->template castKey<KeyType>(id);
            };

            /* First we need to make a copy and then sort both values, vectors
               have to be sorted before the merge. */
            auto currentValuesCopy = currentValues;
            std::ranges::sort(currentValuesCopy, {}, castKey);
            std::ranges::sort(values, {}, castKey);

            // Then merge two vectors
            QList<QVariant> merged;
            merged.reserve(currentValuesCopy.size() + values.size());

            std::ranges::merge(currentValuesCopy, values, std::back_inserter(merged),
                               {}, castKey, castKey);

            // Remove duplicates
            // BUG clang doesn't compile with std::ranges::unique, I don't understand why silverqx
#if (defined(__GNUG__) && !defined(__clang__)) || (defined(_MSC_VER) && _MSC_VER >= 1929)
            auto [first, last] = std::ranges::unique(merged, {}, castKey);
            merged.erase(first, last);
#else
            auto it = ranges::unique(merged, {}, castKey);
            merged.erase(it, ranges::end(merged));
#endif

            currentValues.swap(merged);
        }

        return *this;
    }

    bool SyncChanges::isValidKey(const QString &key) const
    {
        return m_data.contains(key);
    }

    /* Getters */

    QList<QVariant> &SyncChanges::attached()
    {
        return m_data.at(Constants::Attached);
    }

    const QList<QVariant> &SyncChanges::attached() const
    {
        return m_data.at(Constants::Attached);
    }

    QList<QVariant> &SyncChanges::detached()
    {
        return m_data.at(Constants::Detached);
    }

    const QList<QVariant> &SyncChanges::detached() const
    {
        return m_data.at(Constants::Detached);
    }

    QList<QVariant> &SyncChanges::updated()
    {
        return m_data.at(Constants::Updated_);
    }

    const QList<QVariant> &SyncChanges::updated() const
    {
        return m_data.at(Constants::Updated_);
    }

    /* std::map proxy methods */

    SyncChanges::iterator SyncChanges::begin() noexcept
    {
        return m_data.begin();
    }

    SyncChanges::iterator SyncChanges::end() noexcept
    {
        return m_data.end();
    }

    SyncChanges::const_iterator SyncChanges::begin() const noexcept
    {
        return m_data.begin();
    }

    SyncChanges::const_iterator SyncChanges::end() const noexcept
    {
        return m_data.end();
    }

    SyncChanges::const_iterator SyncChanges::cbegin() const noexcept
    {
        return m_data.cbegin();
    }

    SyncChanges::const_iterator SyncChanges::cend() const noexcept
    {
        return m_data.cend();
    }

    SyncChanges::reverse_iterator SyncChanges::rbegin() noexcept
    {
        return m_data.rbegin();
    }

    SyncChanges::reverse_iterator SyncChanges::rend() noexcept
    {
        return m_data.rend();
    }

    SyncChanges::const_reverse_iterator SyncChanges::rbegin() const noexcept
    {
        return m_data.rbegin();
    }

    SyncChanges::const_reverse_iterator SyncChanges::rend() const noexcept
    {
        return m_data.rend();
    }

    SyncChanges::const_reverse_iterator SyncChanges::crbegin() const noexcept
    {
        return m_data.crbegin();
    }

    SyncChanges::const_reverse_iterator SyncChanges::crend() const noexcept
    {
        return m_data.crend();
    }

    /* Capacity */

    SyncChanges::size_type SyncChanges::size() const
    noexcept(std::is_nothrow_invocable_v<decltype (&mapped_type::size), ContainerType &>)
    {
        return static_cast<size_type>(attached().size()) +
               static_cast<size_type>(detached().size()) +
               static_cast<size_type>(updated().size());
    }

    bool SyncChanges::empty() const
    noexcept(std::is_nothrow_invocable_r_v<bool, decltype (&mapped_type::empty),
                                           ContainerType &>)
    {
        return attached().empty() && detached().empty() && updated().empty();
    }

    /* Element access */

    SyncChanges::mapped_type &SyncChanges::at(const key_type &key)
    {
        return m_data.at(key);
    }

    const SyncChanges::mapped_type &SyncChanges::at(const key_type &key) const
    {
        return m_data.at(key);
    }

    SyncChanges::mapped_type &SyncChanges::operator[](const key_type &key)
    {
        return m_data.at(key);
    }

    const SyncChanges::mapped_type &SyncChanges::operator[](const key_type &key) const
    {
        return m_data.at(key);
    }

    /* Modifiers */

    void SyncChanges::clear()
    {
        m_data.at(Constants::Attached).clear();
        m_data.at(Constants::Detached).clear();
        m_data.at(Constants::Updated_).clear();
    }

    void SyncChanges::swap(SyncChanges &right)
    noexcept(std::is_nothrow_swappable_v<ContainerType>)
    {
        if (this == std::addressof(right))
            return;

        m_data.swap(right.m_data);
    }

    /* protected */

    template<typename T>
    T SyncChanges::castKey(const QVariant &key) const
    {
        return key.template value<T>();
    }

    /* Non-member functions */

    /*! Swap the SyncChanges. */
    inline void swap(SyncChanges &left, SyncChanges &right)
    noexcept(noexcept(left.swap(right)))
    {
        left.swap(right);
    }

} // namespace Types

    /*! Alias for the Types::SyncChanges, shortcut alias. */
    using Tiny::Types::SyncChanges; // NOLINT(misc-unused-using-decls)

} // namespace Tiny
} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_TYPES_SYNCCHANGES_HPP
