#ifndef HASMANY_H
#define HASMANY_H

#include "orm/tiny/relations/hasoneormany.h"

#ifdef MANGO_COMMON_NAMESPACE
namespace MANGO_COMMON_NAMESPACE
{
#endif
namespace Orm::Tiny::Relations
{

    template<class Model, class Related>
    class HasMany : public HasOneOrMany<Model, Related>, public ManyRelation
    {
    public:
        /*! Instantiate and initialize a new HasMany instance. */
        static std::unique_ptr<Relation<Model, Related>>
        create(std::unique_ptr<Builder<Related>> &&query, Model &parent,
               const QString &foreignKey, const QString &localKey);

        /*! Initialize the relation on a set of models. */
        QVector<Model> &
        initRelation(QVector<Model> &models, const QString &relation) const override;
        /*! Match the eagerly loaded results to their parents. */
        inline void match(QVector<Model> &models, QVector<Related> results,
                          const QString &relation) const override
        { this->template matchOneOrMany<QVector<Related>>(models, results, relation); }
        /*! Get the results of the relationship. */
        std::variant<QVector<Related>, std::optional<Related>>
        getResults() const override;

    private:
        HasMany(std::unique_ptr<Builder<Related>> &&query,
                const Model &parent,
                const QString &foreignKey, const QString &localKey);
    };

    template<class Model, class Related>
    HasMany<Model, Related>::HasMany(std::unique_ptr<Builder<Related>> &&query,
                                     const Model &parent,
                                     const QString &foreignKey, const QString &localKey)
        : HasOneOrMany<Model, Related>(std::move(query), parent, foreignKey, localKey)
    {}

    template<class Model, class Related>
    std::unique_ptr<Relation<Model, Related>>
    HasMany<Model, Related>::create(std::unique_ptr<Builder<Related>> &&query, Model &parent,
                                    const QString &foreignKey, const QString &localKey)
    {
        auto instance = std::unique_ptr<HasMany<Model, Related>>(
                    new HasMany(std::move(query), parent, foreignKey, localKey));

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

        // TODO add support for default models (trait SupportsDefaultModels) silverqx
//        model.setRelation(relation, getDefaultFor(model));

        return models;
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

} // namespace Orm::Tiny::Relations
#ifdef MANGO_COMMON_NAMESPACE
} // namespace MANGO_COMMON_NAMESPACE
#endif

#endif // HASMANY_H
