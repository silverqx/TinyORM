#pragma once
#ifndef HASMANY_H
#define HASMANY_H

#include "orm/tiny/relations/hasoneormany.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Tiny::Relations
{

    /*! Has many relation. */
    template<class Model, class Related>
    class HasMany :
            public ManyRelation,
            public HasOneOrMany<Model, Related>
    {
    protected:
        /*! Protected constructor. */
        HasMany(std::unique_ptr<Related> &&related, Model &parent,
                const QString &foreignKey, const QString &localKey);

    public:
        /*! Instantiate and initialize a new HasMany instance. */
        static std::unique_ptr<HasMany<Model, Related>>
        instance(std::unique_ptr<Related> &&related, Model &parent,
                 const QString &foreignKey, const QString &localKey);

        /*! Initialize the relation on a set of models. */
        QVector<Model> &
        initRelation(QVector<Model> &models, const QString &relation) const override;

        /*! Match the eagerly loaded results to their parents. */
        void match(QVector<Model> &models, QVector<Related> results,
                   const QString &relation) const override;

        /*! Get the results of the relationship. */
        std::variant<QVector<Related>, std::optional<Related>>
        getResults() const override;

        /* TinyBuilder proxy methods */
        /*! Find multiple models by their primary keys. */
        QVector<Related>
        findMany(const QVector<QVariant> &ids,
                 const QVector<Column> &columns = {ASTERISK}) const;

        /* Others */
        /*! The textual representation of the Relation type. */
        QString relationTypeName() const override;
    };

    template<class Model, class Related>
    HasMany<Model, Related>::HasMany(
            std::unique_ptr<Related> &&related, Model &parent,
            const QString &foreignKey, const QString &localKey
    )
        : HasOneOrMany<Model, Related>(std::move(related), parent, foreignKey, localKey)
    {}

    template<class Model, class Related>
    std::unique_ptr<HasMany<Model, Related>>
    HasMany<Model, Related>::instance(
            std::unique_ptr<Related> &&related, Model &parent,
            const QString &foreignKey, const QString &localKey)
    {
        // Relations have private ctors, std::make_unique() can't be used
        auto instance = std::unique_ptr<HasMany<Model, Related>>(
                    new HasMany(std::move(related), parent, foreignKey, localKey));

        instance->init();

        return instance;
    }

    template<class Model, class Related>
    QVector<Model> &
    HasMany<Model, Related>::initRelation(QVector<Model> &models,
                                          const QString &relation) const
    {
        for (auto &model : models)
            model.template setRelation<Related>(relation, QVector<Related>());

        return models;
    }

    template<class Model, class Related>
    inline void
    HasMany<Model, Related>::match(
            QVector<Model> &models,  QVector<Related> results,
            const QString &relation) const
    {
        this->template matchOneOrMany<QVector<Related>>(models, results, relation);
    }

    template<class Model, class Related>
    std::variant<QVector<Related>, std::optional<Related>>
    HasMany<Model, Related>::getResults() const
    {
        // Model doesn't contain primary key ( eg empty Model instance )
        if (const auto key = this->getParentKey();
            !key.isValid() || key.isNull()
        )
            return QVector<Related>();

        return this->m_query->get();
    }

    template<class Model, class Related>
    QVector<Related>
    HasMany<Model, Related>::findMany(
            const QVector<QVariant> &ids, const QVector<Column> &columns) const
    {
        return this->m_query->findMany(ids, columns);
    }

    template<class Model, class Related>
    inline QString HasMany<Model, Related>::relationTypeName() const
    {
        return "HasMany";
    }

} // namespace Orm::Tiny::Relations
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // HASMANY_H
