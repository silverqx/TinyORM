#pragma once
#ifndef ORM_TINY_RELATIONS_BELONGSTO_HPP
#define ORM_TINY_RELATIONS_BELONGSTO_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/tiny/relations/concerns/comparesrelatedmodels.hpp"
#include "orm/tiny/relations/concerns/supportsdefaultmodels.hpp"
#include "orm/tiny/relations/relation.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny::Relations
{

    /*! Belongs to relation. */
    template<class Model, class Related>
    class BelongsTo : public IsOneRelation,
                      public Relation<Model, Related>,
                      public Concerns::SupportsDefaultModels<Model, Related, BelongsTo>,
                      public Concerns::ComparesRelatedModels<Model, Related, BelongsTo>
    {
        Q_DISABLE_COPY_MOVE(BelongsTo)

        // To access getRelatedKeyFrom()
        friend Concerns::ComparesRelatedModels<Model, Related, BelongsTo>;

        /*! Alias for the NotNull. */
        template<typename T>
        using NotNull = Orm::Utils::NotNull<T>;

    protected:
        /*! Protected constructor. */
        BelongsTo(std::unique_ptr<Related> &&related, Model &child,
                  const QString &foreignKey, const QString &ownerKey, // NOLINT(modernize-pass-by-value)
                  const QString &relationName); // NOLINT(modernize-pass-by-value)

    public:
        /*! Parent Model type. */
        using ModelType = Model;
        /*! Related type. */
        using RelatedType = Related;

        /*! Virtual destructor. */
        ~BelongsTo() override = default;

        /*! Instantiate and initialize a new BelongsTo instance. */
        static std::unique_ptr<BelongsTo<Model, Related>>
        instance(std::unique_ptr<Related> &&related,
                 Model &child, const QString &foreignKey,
                 const QString &ownerKey, const QString &relation);

        /* Relation related operations */
        /*! Set the base constraints on the relation query. */
        void addConstraints() const override;

        /*! Set the constraints for an eager load of the relation. */
        inline void addEagerConstraints(const ModelsCollection<Model> &models) override;
        /*! Set the constraints for an eager load of the relation. */
        inline void addEagerConstraints(const ModelsCollection<Model *> &models) override;

        /*! Initialize the relation on a set of models. */
        inline ModelsCollection<Model> &
        initRelation(ModelsCollection<Model> &models,
                     const QString &relation) const override;
        /*! Initialize the relation on a set of models. */
        inline ModelsCollection<Model *> &
        initRelation(ModelsCollection<Model *> &models,
                     const QString &relation) const override;

        /*! Match the eagerly loaded results to their parents. */
        inline void
        match(ModelsCollection<Model> &models, ModelsCollection<Related> &&results,
              const QString &relation) const override;
        /*! Match the eagerly loaded results to their parents. */
        inline void
        match(ModelsCollection<Model *> &models, ModelsCollection<Related> &&results,
              const QString &relation) const override;

        /*! Get the results of the relationship. */
        std::variant<ModelsCollection<Related>, std::optional<Related>>
        getResults() const override;

        /* Updating relationship */
        /*! Associate the model instance to the given parent. */
        Model &associate(const Related &model) const;
        /*! Associate the model instance to the given parent. */
        Model &associate(const QVariant &id) const;
        /*! Dissociate previously associated model from the given parent. */
        Model &dissociate() const;
        /*! Alias of "dissociate" method. */
        inline Model &disassociate() const;

        /* Getters / Setters */
        /*! Get the child of the relationship. */
        inline const Model &getChild() const noexcept;
        /*! Get the key value of the child's foreign key. */
        inline QVariant getParentKey() const;
        /*! Get the associated key of the relationship. */
        inline const QString &getOwnerKeyName() const noexcept;
        /*! Get the fully qualified associated key of the relationship. */
        inline QString getQualifiedOwnerKeyName() const;
        /*! Get the foreign key of the relationship. */
        inline const QString &getForeignKeyName() const noexcept;
        /*! Get the fully qualified foreign key of the relationship. */
        inline QString getQualifiedForeignKeyName() const;
        /*! Get the name of the relationship. */
        inline const QString &getRelationName() const noexcept;

        /* Others */
        /*! The textual representation of the Relation type. */
        inline const QString &relationTypeName() const override;

    protected:
        /* Relation related operations */
        /*! Gather the keys from a vector of related models. */
        template<SameDerivedCollectionModel<Model> CollectionModel>
        QList<QVariant>
        getEagerModelKeys(const ModelsCollection<CollectionModel> &models) const;

        /*! Build model dictionary keyed by the parent's primary key. */
        QHash<typename Model::KeyType, Related>
        buildDictionary(ModelsCollection<Related> &&results) const; // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)

        /*! Make a new related instance for the given model. */
        inline Related newRelatedInstanceFor(const Model &/*unused*/) const override;

        /* Getters */
        /*! Get the value of the model's foreign key. */
        template<ModelConcept M>
        inline QVariant getRelatedKeyFrom(const M &model) const;

        /* Querying Relationship Existence/Absence */
        /*! Add the constraints for a relationship query. */
        std::unique_ptr<Builder<Related>>
        getRelationExistenceQuery( // NOLINT(google-default-arguments)
                std::unique_ptr<Builder<Related>> &&query,
                const Builder<Model> &parentQuery,
                const QList<Column> &columns = {ASTERISK}) const override;

        /*! The child model instance of the relation. */
        NotNull<Model *> m_child;
        /*! The foreign key of the parent model. */
        QString m_foreignKey;
        /*! The associated key on the parent model. */
        QString m_ownerKey;
        /*! The name of the relationship. */
        QString m_relationName;
        /*! The count of self joins. */
        T_THREAD_LOCAL
        inline static int selfJoinCount = 0;

    private:
        /* Relation related operations */
        /*! Set the constraints for an eager load of the relation, common code. */
        template<SameDerivedCollectionModel<Model> CollectionModel>
        void addEagerConstraintsInternal(const ModelsCollection<CollectionModel> &models);

        /*! Initialize the relation on a set of models, common code. */
        template<SameDerivedCollectionModel<Model> CollectionModel>
        ModelsCollection<CollectionModel> &
        initRelationInternal(ModelsCollection<CollectionModel> &models,
                             const QString &relation) const;
        /*! Match the eagerly loaded results to their parents. */
        template<SameDerivedCollectionModel<Model> CollectionModel>
        void matchInternal(
                ModelsCollection<CollectionModel> &models,
                ModelsCollection<Related> &&results, const QString &relation) const;
    };

    /* protected */

    template<class Model, class Related>
    BelongsTo<Model, Related>::BelongsTo(
            std::unique_ptr<Related> &&related, Model &child,
            const QString &foreignKey, const QString &ownerKey, // NOLINT(modernize-pass-by-value)
            const QString &relationName // NOLINT(modernize-pass-by-value)
    )
        : Relation<Model, Related>(std::move(related), child)
        /* In the underlying base relationship class, this variable is referred to as
           the "parent" since most relationships are not inversed. But, since this
           one is we will create a "child" variable for much better readability. */
        , m_child(this->m_parent)
        , m_foreignKey(foreignKey)
        , m_ownerKey(ownerKey)
        , m_relationName(relationName)
    {}

    /* public */

    template<class Model, class Related>
    std::unique_ptr<BelongsTo<Model, Related>>
    BelongsTo<Model, Related>::instance(
            std::unique_ptr<Related> &&related, Model &child,
            const QString &foreignKey, const QString &ownerKey, const QString &relation)
    {
        // Relations have private ctors, std::make_unique() can't be used
        auto instance =
                std::unique_ptr<BelongsTo<Model, Related>>(
                    new BelongsTo(std::move(related), child, foreignKey,
                                  ownerKey, relation));

        instance->init();

        return instance;
    }

    /* Relation related operations */

    template<class Model, class Related>
    void BelongsTo<Model, Related>::addConstraints() const
    {
        if (!this->constraints)
            return;

        /* For belongs to relationships, which are essentially the inverse of has-one
           or has-many relationships, we need to actually query on the primary key
           of the related models matching on the foreign key that's on a parent. */
        const auto &table = this->m_related->getTable();

        this->m_query->where(DOT_IN.arg(table, m_ownerKey), EQ,
                             m_child->getAttribute(m_foreignKey));
    }

    template<class Model, class Related>
    void
    BelongsTo<Model, Related>::addEagerConstraints(const ModelsCollection<Model> &models)
    {
        addEagerConstraintsInternal(models);
    }

    template<class Model, class Related>
    void BelongsTo<Model, Related>::addEagerConstraints(
            const ModelsCollection<Model *> &models)
    {
        addEagerConstraintsInternal(models);
    }

    template<class Model, class Related>
    ModelsCollection<Model> &
    BelongsTo<Model, Related>::initRelation(ModelsCollection<Model> &models,
                                            const QString &relation) const
    {
        return initRelationInternal(models, relation);
    }

    template<class Model, class Related>
    ModelsCollection<Model *> &
    BelongsTo<Model, Related>::initRelation(ModelsCollection<Model *> &models,
                                            const QString &relation) const
    {
        return initRelationInternal(models, relation);
    }

    template<class Model, class Related>
    void BelongsTo<Model, Related>::match(
            ModelsCollection<Model> &models, ModelsCollection<Related> &&results,
            const QString &relation) const
    {
        matchInternal(models, std::move(results), relation);
    }

    template<class Model, class Related>
    void BelongsTo<Model, Related>::match(
            ModelsCollection<Model *> &models, ModelsCollection<Related> &&results,
            const QString &relation) const
    {
        matchInternal(models, std::move(results), relation);
    }

    template<class Model, class Related>
    QHash<typename Model::KeyType, Related>
    BelongsTo<Model, Related>::buildDictionary(ModelsCollection<Related> &&results) const // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
    {
        QHash<typename Model::KeyType, Related> dictionary;
        dictionary.reserve(results.size());

        /*! Build model dictionary keyed by the parent's primary key. */
        for (auto &&result : results)
            dictionary.emplace(result.getAttribute(m_ownerKey)
                               .template value<typename Model::KeyType>(),
                               std::move(result));

        return dictionary;
    }

    template<class Model, class Related>
    std::variant<ModelsCollection<Related>, std::optional<Related>>
    BelongsTo<Model, Related>::getResults() const
    {
        // Model doesn't contain foreign key ( eg empty Model instance )
        if (const auto foreignKey = m_child->getAttribute(m_foreignKey);
            !foreignKey.isValid() || foreignKey.isNull()
        )
            return this->getDefaultFor(*m_child);

        // NRVO doesn't kick in so I have to move
        auto first = this->m_query->first();

        return first ? std::move(first) : this->getDefaultFor(*m_child);
    }

    /* Updating relationship */

    template<class Model, class Related>
    Model &BelongsTo<Model, Related>::associate(const Related &model) const
    {
        m_child->setAttribute(m_foreignKey,
                              model.getAttribute(m_ownerKey));

        m_child->template setRelation<Related>(m_relationName, model);

        return *m_child;
    }

    // FEATURE dilemma primarykey, Model::KeyType vs QVariant silverqx
    template<class Model, class Related>
    Model &BelongsTo<Model, Related>::associate(const QVariant &id) const
    {
        m_child->setAttribute(m_foreignKey, id);

        // FEATURE relations, check if relation is loaded and if has the same id, if so, then don't unset relation; many months later I don't know if this is a good idea, how it will behave if the relation will not be unset? and comparing only ID is not enough there can be changed attributes or relations, I think best to no remove this and if I decide to remove it test it carefully silverqx
        m_child->unsetRelation(m_relationName);

        return *m_child;
    }

    template<class Model, class Related>
    Model &BelongsTo<Model, Related>::dissociate() const
    {
        // TEST Model::save with null key silverqx
        // FEATURE dilemma primarykey, Model::KeyType vs QVariant, set to null, for Qt6 it will be (QVariant(QMetaType(qMetaTypeId<Model::KeyType>())))) ; ALSO current problem is, that I check that foreignKey !isValid || isNull, but when QVariant with type (Model::KeyType) and also with null is created by the above commands, then it is still null (isNull == true), but is considered as !!VALID!! (isValid == true) silverqx
        m_child->setAttribute(m_foreignKey, {});

        // TEST operations that are related on the Model::m_relation data member how they behave, when m_relations value contains the std::nullopt value silverqx
        return m_child->template setRelation<Related>(m_relationName, std::nullopt);
    }

    template<class Model, class Related>
    Model &BelongsTo<Model, Related>::disassociate() const
    {
        return dissociate();
    }

    /* Getters / Setters */

    template<class Model, class Related>
    const Model &BelongsTo<Model, Related>::getChild() const noexcept
    {
        return *m_child;
    }

    template<class Model, class Related>
    QVariant BelongsTo<Model, Related>::getParentKey() const
    {
        return m_child->getAttribute(m_foreignKey);
    }

    template<class Model, class Related>
    const QString &BelongsTo<Model, Related>::getOwnerKeyName() const noexcept
    {
        return m_ownerKey;
    }

    template<class Model, class Related>
    QString BelongsTo<Model, Related>::getQualifiedOwnerKeyName() const
    {
        return this->m_related->qualifyColumn(m_ownerKey);
    }

    template<class Model, class Related>
    const QString &BelongsTo<Model, Related>::getForeignKeyName() const noexcept
    {
        return m_foreignKey;
    }

    template<class Model, class Related>
    QString BelongsTo<Model, Related>::getQualifiedForeignKeyName() const
    {
        return m_child->qualifyColumn(m_foreignKey);
    }

    template<class Model, class Related>
    const QString &BelongsTo<Model, Related>::getRelationName() const noexcept
    {
        return m_relationName;
    }

    /* Others */

    template<class Model, class Related>
    const QString &BelongsTo<Model, Related>::relationTypeName() const
    {
        static const auto cached = QStringLiteral("BelongsTo");
        return cached;
    }

    /* protected */

    /* Relation related operations */

    template<class Model, class Related>
    template<SameDerivedCollectionModel<Model> CollectionModel>
    QList<QVariant>
    BelongsTo<Model, Related>::getEagerModelKeys(
            const ModelsCollection<CollectionModel> &models) const
    {
        QList<QVariant> keys;
        keys.reserve(models.size());

        /*! Const Model type used in the for-ranged loops. */
        using ConstModelLoopType = typename ModelsCollection<CollectionModel>::
                                            ConstModelLoopType;

        /* First we need to gather all of the keys from the parent models so we know what
           to query for via the eager loading query. We will add them to the vector then
           execute a "where in" statement to gather up all of those related records. */
        for (ConstModelLoopType model : models) {
            auto value = Relation<Model,Related>::toPointer(model)
                         ->getAttribute(m_foreignKey);

            if (value.isValid() && !value.isNull())
                // TODO add support for non-int primary keys, ranges::actions doesn't accept QVariant container silverqx
                keys << std::move(value);
        }

        return keys |= ranges::actions::sort(ranges::less {}, [](const auto &key_)
        {
            return key_.template value<typename Model::KeyType>();
        })
                | ranges::actions::unique;
    }

    template<class Model, class Related>
    Related
    BelongsTo<Model, Related>::newRelatedInstanceFor(const Model &/*unused*/) const
    {
        return this->m_related->newInstance();
    }

    /* Getters */

    template<class Model, class Related>
    template<ModelConcept M>
    QVariant BelongsTo<Model, Related>::getRelatedKeyFrom(const M &model) const
    {
        return model.getAttribute(m_ownerKey);
    }

    /* Querying Relationship Existence/Absence */

    template<class Model, class Related>
    std::unique_ptr<Builder<Related>>
    BelongsTo<Model, Related>::getRelationExistenceQuery( // NOLINT(google-default-arguments)
            std::unique_ptr<Builder<Related>> &&query,
            const Builder<Model> &/*parentQuery*/,
            const QList<Column> &columns) const
    {
        // CUR1 finish self query silverqx
//        if (query->getQuery()->from == parentQuery.getQuery()->from)
//            return this->getRelationExistenceQueryForSelfRelation(query, parentQuery,
//                                                                  columns);

        query->select(columns).whereColumnEq(getQualifiedForeignKeyName(),
                                             query->qualifyColumn(m_ownerKey));

        return std::move(query);
    }

    /* private */

    /* Relation related operations */

    template<class Model, class Related>
    template<SameDerivedCollectionModel<Model> CollectionModel>
    void BelongsTo<Model, Related>::addEagerConstraintsInternal(
            const ModelsCollection<CollectionModel> &models)
    {
        /* We'll grab the primary key name of the related models since it could be set to
           a non-standard name and not "id". We will then construct the constraint for
           our eagerly loading query so it returns the proper models from execution. */
        this->whereInEager(DOT_IN.arg(this->m_related->getTable(), m_ownerKey),
                           getEagerModelKeys(models));
    }

    template<class Model, class Related>
    template<SameDerivedCollectionModel<Model> CollectionModel>
    ModelsCollection<CollectionModel> &
    BelongsTo<Model, Related>::initRelationInternal(
            ModelsCollection<CollectionModel> &models,
            const QString &relation) const
    {
        /*! Model type used in the for-ranged loops. */
        using ModelLoopType = typename ModelsCollection<CollectionModel>::ModelLoopType;

        for (ModelLoopType model : models)
            Relation<Model,Related>::toPointer(model)
                    ->template setRelation<Related>(relation,
                                                    this->getDefaultFor(model));

        return models;
    }

    template<class Model, class Related>
    template<SameDerivedCollectionModel<Model> CollectionModel>
    void BelongsTo<Model, Related>::matchInternal(
            ModelsCollection<CollectionModel> &models,
            ModelsCollection<Related> &&results, const QString &relation) const
    {
        /* First we will get to build a dictionary of the child models by their primary
           key of the relationship, then we can easily match the children back onto
           the parents using that dictionary and the primary key of the children. */
        const auto dictionary = buildDictionary(std::move(results));

        /*! Model type used in the for-ranged loops. */
        using ModelLoopType = typename ModelsCollection<CollectionModel>::ModelLoopType;

        /* Once we have the dictionary constructed, we can loop through all the parents
           and match back onto their children using these keys of the dictionary and
           the primary key of the children to map them onto the correct instances. */
        for (ModelLoopType model : models) {
            auto *const modelPointer = Relation<Model,Related>::toPointer(model);

            if (const auto foreignKey = modelPointer->getAttribute(m_foreignKey)
                                        .template value<typename Model::KeyType>();
                dictionary.contains(foreignKey)
            )
                modelPointer->setRelation(
                            relation,
                            std::make_optional<Related>(
                                /* Don't move here! as one Related model can be set
                                   on more parent models. */
                                dictionary.find(foreignKey).value()));
        }
    }

} // namespace Orm::Tiny::Relations

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_RELATIONS_BELONGSTO_HPP
