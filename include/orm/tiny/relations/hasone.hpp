#pragma once
#ifndef HASONE_H
#define HASONE_H

#include "orm/tiny/relations/concerns/supportsdefaultmodels.hpp"
#include "orm/tiny/relations/hasoneormany.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Tiny::Relations
{

    /*! Has one relation. */
    template<class Model, class Related>
    class HasOne :
            public OneRelation,
            public HasOneOrMany<Model, Related>,
            public Concerns::SupportsDefaultModels<Model, Related>
    {
    protected:
        /*! Protected constructor. */
        HasOne(std::unique_ptr<Related> &&related, Model &parent,
               const QString &foreignKey, const QString &localKey);

    public:
        /*! Instantiate and initialize a new HasOne instance. */
        static std::unique_ptr<HasOne<Model, Related>>
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

        /* Others */
        /*! The textual representation of the Relation type. */
        QString relationTypeName() const override;

    protected:
        /*! Make a new related instance for the given model. */
        Related newRelatedInstanceFor(const Model &) const override;
    };

    template<class Model, class Related>
    HasOne<Model, Related>::HasOne(
            std::unique_ptr<Related> &&related, Model &parent,
            const QString &foreignKey, const QString &localKey
    )
        : HasOneOrMany<Model, Related>(std::move(related), parent, foreignKey, localKey)
    {}

    template<class Model, class Related>
    std::unique_ptr<HasOne<Model, Related>>
    HasOne<Model, Related>::instance(
            std::unique_ptr<Related> &&related, Model &parent,
            const QString &foreignKey, const QString &localKey)
    {
        auto instance = std::unique_ptr<HasOne<Model, Related>>(
                    new HasOne(std::move(related), parent, foreignKey, localKey));

        instance->init();

        return instance;
    }

    template<class Model, class Related>
    QVector<Model> &
    HasOne<Model, Related>::initRelation(QVector<Model> &models,
                                         const QString &relation) const
    {
        for (auto &model : models)
            model.template setRelation<Related>(relation,
                                                this->getDefaultFor(model));

        return models;
    }

    template<class Model, class Related>
    inline void
    HasOne<Model, Related>::match(
            QVector<Model> &models, QVector<Related> results,
            const QString &relation) const
    {
        this->template matchOneOrMany<std::optional<Related>>(models, results, relation);
    }

    template<class Model, class Related>
    std::variant<QVector<Related>, std::optional<Related>>
    HasOne<Model, Related>::getResults() const
    {
        // Model doesn't contain primary key ( eg empty Model instance )
        if (const auto key = this->getParentKey();
            !key.isValid() || key.isNull()
        )
            return this->getDefaultFor(this->m_parent);

        // NRVO should kick in, I leave it const
        const auto first = this->m_query->first();

        return first ? first : this->getDefaultFor(this->m_parent);
    }

    template<class Model, class Related>
    inline QString HasOne<Model, Related>::relationTypeName() const
    {
        return "HasOne";
    }

    template<class Model, class Related>
    inline Related
    HasOne<Model, Related>::newRelatedInstanceFor(const Model &parent) const
    {
        return this->m_related->newInstance().setAttribute(
            this->getForeignKeyName(), parent[this->m_localKey]
        );
    }

} // namespace Orm::Tiny::Relations
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // HASONE_H
