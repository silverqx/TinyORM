#pragma once
#ifndef ORM_TINY_RELATIONS_HASMANY_HPP
#define ORM_TINY_RELATIONS_HASMANY_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/tiny/relations/hasoneormany.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny::Relations
{

    /*! Has many relation. */
    template<class Model, class Related>
    class HasMany : public IsManyRelation,
                    public HasOneOrMany<Model, Related>
    {
        Q_DISABLE_COPY(HasMany)

    protected:
        /*! Protected constructor. */
        HasMany(std::unique_ptr<Related> &&related, Model &parent,
                const QString &foreignKey, const QString &localKey);

    public:
        /*! Parent Model type. */
        using ModelType = Model;
        /*! Related type. */
        using RelatedType = Related;

        /*! Virtual destructor. */
        inline ~HasMany() override = default;

        /*! Instantiate and initialize a new HasMany instance. */
        static std::unique_ptr<HasMany<Model, Related>>
        instance(std::unique_ptr<Related> &&related, Model &parent,
                 const QString &foreignKey, const QString &localKey);

        /* Relation related operations */
        /*! Initialize the relation on a set of models. */
        QVector<Model> &
        initRelation(QVector<Model> &models, const QString &relation) const override;

        /*! Match the eagerly loaded results to their parents. */
        inline void match(QVector<Model> &models, QVector<Related> &&results,
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
        inline const QString &relationTypeName() const override;
    };

    /* protected */

    template<class Model, class Related>
    HasMany<Model, Related>::HasMany(
            std::unique_ptr<Related> &&related, Model &parent,
            const QString &foreignKey, const QString &localKey
    )
        : HasOneOrMany<Model, Related>(std::move(related), parent, foreignKey, localKey)
    {}

    /* public */

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

    /* Relation related operations */

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
    void HasMany<Model, Related>::match(
            QVector<Model> &models, QVector<Related> &&results,
            const QString &relation) const
    {
        this->template matchOneOrMany<QVector<Related>>(models, std::move(results),
                                                        relation);
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

    /* TinyBuilder proxy methods */

    template<class Model, class Related>
    QVector<Related>
    HasMany<Model, Related>::findMany(
            const QVector<QVariant> &ids, const QVector<Column> &columns) const
    {
        return this->m_query->findMany(ids, columns);
    }

    /* Others */

    template<class Model, class Related>
    const QString &HasMany<Model, Related>::relationTypeName() const
    {
        static const auto cached = QStringLiteral("HasMany");
        return cached;
    }

} // namespace Orm::Tiny::Relations

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_RELATIONS_HASMANY_HPP
