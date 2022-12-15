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

//        /*! Get a size of the greatest element in the container. */
//        template<QStringContainer T, typename SizeType = typename T::size_type>
//        static SizeType
//        maxElementSize(const T &container, typename T::size_type addToElement = 0);

        /*! Count all string sizes in the given container and optionally add value to
            every string. */
        template<QStringContainer T, typename SizeType = typename T::size_type>
        static SizeType
        countStringSizes(const T &container, typename T::size_type addToElement = 0);
    };

    template<JoinContainer T, DelimiterConcept D>
    QString Container::join(const T &container, D &&delimiter)
    {
        QString columnized;
        // Estimate a size to avoid resizing, 7 for an item and 2 for the delimiter
        columnized.reserve(static_cast<QString::size_type>(container.size()) * (7 + 2));

        if (container.empty())
            return columnized;

        auto end = container.end();
        --end;
        auto it = container.begin();

        for (; it != end; ++it)
            columnized += Constants::NOSPACE.arg(*it).arg(std::forward<D>(delimiter));

        if (it == end)
            columnized += *it;

        return columnized;
    }

//    template<QStringContainer T, typename SizeType>
//    SizeType
//    Container::maxElementSize(const T &container,
//                              const typename T::size_type addToElement)
//    {
//        // Nothing to do
//        if (container.empty())
//            return 0;

//        SizeType result = 0;

//        for (const auto &element : container)
//            if (const auto elementSize = element.size();
//                elementSize > result
//            )
//                result = elementSize;

//        /* This is the reason for the addToElement argument, this algorithm returns 0,
//           if the result is 0. */
//        if (result == 0)
//            return 0;

//        return result + addToElement;
//    }

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

} // namespace Orm::Utils

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_UTILS_CONTAINER_HPP
