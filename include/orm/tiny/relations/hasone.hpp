#ifndef HASONE_H
#define HASONE_H

#include "orm/tiny/relations/hasoneormany.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Tiny::Relations
{

    template<class Model, class Related>
    class HasOne : public HasOneOrMany<Model, Related>, OneRelation
    {
    protected:
        HasOne(std::unique_ptr<Related> &&related, Model &parent,
               const QString &foreignKey, const QString &localKey);

    public:
        /*! Instantiate and initialize a new HasOne instance. */
        static std::unique_ptr<Relation<Model, Related>>
        instance(std::unique_ptr<Related> &&related, Model &parent,
                 const QString &foreignKey, const QString &localKey);

        /*! Initialize the relation on a set of models. */
        QVector<Model> &
        initRelation(QVector<Model> &models, const QString &relation) const override;

        /*! Match the eagerly loaded results to their parents. */
        inline void match(QVector<Model> &models, QVector<Related> results,
                          const QString &relation) const override
        { this->template matchOneOrMany<std::optional<Related>>(models, results,
                                                                relation); }

        /*! Get the results of the relationship. */
        std::variant<QVector<Related>, std::optional<Related>>
        getResults() const override;

    protected:
        /*! The textual representation of the Relation type. */
        inline QString relationTypeName() const override
        { return "HasOne"; };
    };

    template<class Model, class Related>
    HasOne<Model, Related>::HasOne(
            std::unique_ptr<Related> &&related, Model &parent,
            const QString &foreignKey, const QString &localKey
    )
        : HasOneOrMany<Model, Related>(std::move(related), parent, foreignKey, localKey)
    {}

    template<class Model, class Related>
    std::unique_ptr<Relation<Model, Related>>
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
        // FEATURE default models, add support for default models (trait SupportsDefaultModels) silverqx
        for (auto &model : models)
            model.template setRelation<Related>(relation, std::nullopt);

        return models;
    }

    template<class Model, class Related>
    std::variant<QVector<Related>, std::optional<Related>>
    HasOne<Model, Related>::getResults() const
    {
        // Model doesn't contain primary key ( eg empty Model instance )
        if (const auto key = this->getParentKey();
            !key.isValid() || key.isNull()
        )
            return std::nullopt;

        // NRVO should kick in, I leave it const
        const auto first = this->m_query->first();

        return first ? first : std::nullopt;
    }

} // namespace Orm::Tiny::Relations
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // HASONE_H
