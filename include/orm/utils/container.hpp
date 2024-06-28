#pragma once
#ifndef ORM_UTILS_CONTAINER_HPP
#define ORM_UTILS_CONTAINER_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/ormconcepts.hpp" // IWYU pragma: keep
#include "orm/utils/integralcast.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Utils
{

    /*! Containers related library class. */
    class Container
    {
        Q_DISABLE_COPY_MOVE(Container)

    public:
        /*! Deleted default constructor, this is a pure library class. */
        Container() = delete;
        /*! Deleted destructor. */
        ~Container() = delete;

        /*! Convert a string container into a (comma) delimited string. */
        template<JoinContainer T, DelimiterConcept D = const QString &>
        static QString
        join(const T &container, const D &delimiter = Constants::COMMA);

        /*! Count all string sizes in the given container and optionally add value to
            every string. */
        template<QStringContainer T, typename SizeType = typename T::size_type>
        static SizeType
        countStringSizes(const T &container, typename T::size_type addToElement = 0);

    private:
        /*! Get the delimiter size (returns 1 for the QChar). */
        template<typename SizeType = QString::size_type, DelimiterConcept D>
        static SizeType delimiterSize(const D &delimiter);
    };

    template<JoinContainer T, DelimiterConcept D>
    QString Container::join(const T &container, const D &delimiter)
    {
        // Nothing to join
        if (container.empty())
            return {};

        QString joined;
        // +4 serves as a reserve (for the reserve() 😂)
        const auto delimiterSize_ = delimiterSize<typename T::size_type>(delimiter);
        // Can't be removed as T can be Qt or std container type (different size_type)
        if constexpr (std::is_same_v<QString::size_type, typename T::size_type>)
            joined.reserve(countStringSizes(container, delimiterSize_ + 4));
        else
            joined.reserve(IntegralCast<QString::size_type>(
                               countStringSizes(container, delimiterSize_ + 4)));

        auto it = container.cbegin();
        const auto end = container.cend();

        // Don't prepend a delimiter before the first item
        if (it != end) {
            joined.append(*it);
            ++it;
        }

        while (it != end) {
            // These append-s() are better for performance
            // No need to use the if statement if a delimiter is empty or null
            joined.append(delimiter)
                  .append(*it);
            ++it;
        }

        Q_ASSERT(it == end);

        return joined;
    }

    template<QStringContainer T, typename SizeType>
    SizeType
    Container::countStringSizes(const T &container,
                                const typename T::size_type addToElement)
    {
        // Nothing to do
        if (container.empty())
            return 0;

        SizeType size = 0;

        for (const auto &string : container)
            // Can't be removed as T can be Qt or std container type (different size_type)
            if constexpr (std::is_same_v<SizeType, typename T::value_type::size_type>)
                size += string.size() + addToElement;
            else
                size += static_cast<SizeType>(string.size()) + addToElement;

        return size;
    }

    /* private */

    template<typename SizeType, DelimiterConcept D>
    SizeType Container::delimiterSize(const D &delimiter)
    {
        if constexpr (std::is_constructible_v<QChar, D>)
            return 1;
        else
            return static_cast<SizeType>(QString(delimiter).size());
    }

} // namespace Orm::Utils

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_UTILS_CONTAINER_HPP
