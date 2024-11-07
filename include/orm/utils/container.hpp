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
        /*! The behavior of joining the container if an element is empty. */
        enum struct JoinBehavior : quint8
        {
            /*! Keep empty elements. */
            cKeepEmptyParts,
            /*! Skip empty elements (don't include them in the result). */
            cSkipEmptyParts,
        };
        /*! Expose the JoinBehavior enum. */
        using enum JoinBehavior;

        /*! Deleted default constructor, this is a pure library class. */
        Container() = delete;
        /*! Deleted destructor. */
        ~Container() = delete;

        /*! Convert a string container into a (comma) delimited string. */
        template<JoinBehavior J = cKeepEmptyParts, JoinContainer T,
                 DelimiterConcept D = QString>
        static QString
        join(const T &container, const D &delimiter = Constants::COMMA);

        /*! Count all string sizes in the given container and optionally add value to
            every string. */
        template<QStringContainer T, typename SizeType = T::size_type>
        static SizeType
        countStringSizes(const T &container, T::size_type addToElement = 0);

    private:
        /*! Get the delimiter size (returns 1 for the QChar). */
        template<typename SizeType = QString::size_type, DelimiterConcept D>
        static SizeType delimiterSize(const D &delimiter);
    };

    /* public */

    template<Container::JoinBehavior J, JoinContainer T, DelimiterConcept D>
    QString Container::join(const T &container, const D &delimiter) // Don't use forwardning reference here
    {
        // Nothing to join
        if (container.empty())
            return {};

        /*! Alias for the T size_type (can be anything, most std::size_t or qint64). */
        using TSizeType = T::size_type;

        QString joined;
        // +4 serves as a reserve (for the reserve() 😂)
        const auto delimiterSize_ = delimiterSize<TSizeType>(delimiter);
        // Can't be removed as T can be Qt or std container type (different size_type)
        if constexpr (std::is_same_v<QString::size_type, TSizeType>)
            joined.reserve(countStringSizes(container, delimiterSize_ + 4));
        else
            joined.reserve(IntegralCast<QString::size_type>(
                               countStringSizes(container, delimiterSize_ + 4)));

        auto it = container.cbegin();
        const auto end = container.cend();

        // Don't prepend a delimiter before the first item
        if (it != end) {
            if constexpr (J == cSkipEmptyParts) {
                if (!it->isEmpty())
                    joined.append(*it);
            }
            else
                joined.append(*it);

            ++it;
        }

        while (it != end) {
            /* Made it constexpr for performance reasons (nano-optimization). It makes
               20ms per 1'000'000 loop, but it still makes sense because this method is
               invoked for every one parsed query! */
            if constexpr (J == cSkipEmptyParts)
                if (it->isEmpty()) {
                    ++it;
                    continue;
                }

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
    Container::countStringSizes(const T &container, const T::size_type addToElement)
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
        /* Don't use the std::is_constructible_v<> here as it also considers explicit
           constructors and the DelimiterConcept uses std::convertible_to<> too. */
        if constexpr (std::is_convertible_v<D, QChar>)
            return 1; // static_cast<> is not needed here regardless of SizeType
        // To avoid QString(delimiter) and doing copy if D is QString
        else if (std::is_same_v<D, QString>)
            return static_cast<SizeType>(delimiter.size());
        /* QString(delimiter) is here for safety as we are using std::convertible_to
           concept, so the type can be something else than QString. */
        else
            return static_cast<SizeType>(QString(delimiter).size());
    }

} // namespace Orm::Utils

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_UTILS_CONTAINER_HPP
