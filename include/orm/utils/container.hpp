#pragma once
#ifndef ORM_UTILS_CONTAINER_HPP
#define ORM_UTILS_CONTAINER_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/constants.hpp"
#include "orm/ormconcepts.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Utils
{

    /*! Containers related library class. */
    class Container
    {
        Q_DISABLE_COPY(Container)

    public:
        /*! Deleted default constructor, this is a pure library class. */
        Container() = delete;
        /*! Deleted destructor. */
        ~Container() = delete;

        /*! Convert a string container into a (comma) delimited string. */
        template<JoinContainer T, DelimiterConcept D = const QString &>
        static QString
        join(const T &container, D &&delimiter = Constants::COMMA);

        /*! Count all string sizes in the given container and optionally add value to
            every string. */
        template<QStringContainer T, typename SizeType = typename T::size_type>
        static SizeType
        countStringSizes(const T &container, typename T::size_type addToElement = 0);

    private:
        /*! Get the delimiter size (returns 1 for the QChar). */
        template<DelimiterConcept D>
        static QString::size_type delimiterSize(D &&delimiter);
    };

    template<JoinContainer T, DelimiterConcept D>
    QString Container::join(const T &container, D &&delimiter)
    {
        QString columnized;

        // Nothing to join
        if (container.empty())
            return columnized;

        // +4 serves as a reserve (for the reserve() 😂)
        columnized.reserve(static_cast<QString::size_type>(
                               countStringSizes(container,
                                                delimiterSize(delimiter) + 4)));

        auto end = container.end();
        --end;
        auto it = container.begin();

        for (; it != end; ++it)
            columnized += Constants::NOSPACE.arg(*it).arg(std::forward<D>(delimiter));

        if (it == end)
            columnized += *it;

        return columnized;
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
            size += string.size() + addToElement;

        return size;
    }

    /* private */

    template<DelimiterConcept D>
    QString::size_type Container::delimiterSize(D &&delimiter)
    {
        if constexpr (std::is_constructible_v<D, QChar>)
            return 1;
        else
            return QString(delimiter).size();
    }

} // namespace Orm::Utils

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_UTILS_CONTAINER_HPP
