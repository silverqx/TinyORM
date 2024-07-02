#pragma once
#ifndef TINYUTILS_COMMON_COLLECTION_HPP
#define TINYUTILS_COMMON_COLLECTION_HPP

#include "orm/tiny/types/modelscollection.hpp"

namespace Orm::Tiny::Types
{
    template<DerivedCollectionModel Model>
    class ModelsCollection;
}

namespace TestUtils::Common
{

    /*! Common code for tst_collection_xyz auto tests, library class. */
    class Collection final
    {
        Q_DISABLE_COPY_MOVE(Collection)

        /*! Alias for the ModelsCollection. */
        template<typename T>
        using ModelsCollection = Orm::Tiny::Types::ModelsCollection<T>;

    public:
        /*! Deleted default constructor, this is a pure library class. */
        Collection() = delete;
        /*! Deleted destructor. */
        ~Collection() = delete;

        /*! Orm::Column constant "album_id" (perf. reason, one time initialization). */
        inline static const Orm::Column album_id {std::in_place_index<0>, "album_id"};
        /*! QString constant "album_id" (perf. reason, one time initialization). */
        inline static const auto album_id_s     = QStringLiteral("album_id");
        /*! QString constant "albumImages" (perf. reason, one time initialization). */
        inline static const auto albumImages    = QStringLiteral("albumImages");
        /*! QString constant "ext" (perf. reason, one time initialization). */
        inline static const auto ext            = QStringLiteral("ext");

        /*! Verify primary key IDs in the given collection. */
        template<typename M>
        static bool
        verifyIds(const ModelsCollection<M *> &actual,
                  const std::unordered_set<quint64> &expected);
        /*! Verify primary key IDs in the given collection. */
        template<typename M>
        static bool
        verifyIds(const ModelsCollection<M> &actual,
                  const std::unordered_set<quint64> &expected);
        /*! Verify primary key IDs in the given vector. */
        template<typename M>
        static bool
        verifyIds(const QList<M> &actual,
                  const std::unordered_set<quint64> &expected);
        /*! Verify primary key IDs in the given vector. */
        inline static bool
        verifyIds(const QList<quint64> &actual,
                  const std::unordered_set<quint64> &expected);
        /*! Verify primary key IDs in the given vector. */
        template<typename T>
        static bool
        verifyIdsQVariant(const QList<QVariant> &actual,
                          const std::unordered_set<T> &expected);

        /*! Verify attribute values in the given vector. */
        template<typename T, typename M>
        static bool
        verifyAttributeValues(
                const QString &attribute, const ModelsCollection<M> &actual,
                const std::unordered_set<T> &expected);

        /*! Get the value of the model's primary key casted to the Derived::KeyType. */
        template<typename M>
        static typename M::KeyType getKeyCasted(M *model);
        /*! Get the value of the model's primary key casted to the Derived::KeyType. */
        template<typename M>
        static typename M::KeyType getKeyCasted(const M &model);
    };

    /* public */

    template<typename M>
    bool Collection::verifyIds(const ModelsCollection<M *> &actual,
                               const std::unordered_set<quint64> &expected)
    {
        return std::ranges::all_of(actual, [&expected](M *const model)
        {
            return expected.contains(getKeyCasted(model));
        });
    }

    template<typename M>
    bool Collection::verifyIds(const ModelsCollection<M> &actual,
                               const std::unordered_set<quint64> &expected)
    {
        return std::ranges::all_of(actual, [&expected](const M &model)
        {
            return expected.contains(getKeyCasted(model));
        });
    }

    template<typename M>
    bool Collection::verifyIds(const QList<M> &actual,
                               const std::unordered_set<quint64> &expected)
    {
        return std::ranges::all_of(actual, [&expected](const M &model)
        {
            return expected.contains(getKeyCasted(model));
        });
    }

    bool Collection::verifyIds(const QList<quint64> &actual,
                               const std::unordered_set<quint64> &expected)
    {
        return std::ranges::all_of(actual, [&expected](const quint64 id)
        {
            return expected.contains(id);
        });
    }

    template<typename T>
    bool Collection::verifyIdsQVariant(const QList<QVariant> &actual,
                                       const std::unordered_set<T> &expected)
    {
        return std::ranges::all_of(actual, [&expected](const QVariant &id)
        {
            return expected.contains(id.template value<T>());
        });
    }

    template<typename T, typename M>
    bool Collection::verifyAttributeValues(
            const QString &attribute, const ModelsCollection<M> &actual,
            const std::unordered_set<T> &expected)
    {
        return std::ranges::all_of(actual,
                                   [&attribute, &expected](const M &model)
        {
            return expected.contains(model.template getAttribute<T>(attribute));
        });
    }

    template<typename M>
    typename M::KeyType
    Collection::getKeyCasted(M *const model)
    {
        return model->getKey().template value<typename M::KeyType>();
    }

    template<typename M>
    typename M::KeyType
    Collection::getKeyCasted(const M &model)
    {
        return model.getKey().template value<typename M::KeyType>();
    }

} // namespace TestUtils::Common

#endif // TINYUTILS_COMMON_COLLECTION_HPP
