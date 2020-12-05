#ifndef BASEMODEL_H
#define BASEMODEL_H

#include <range/v3/algorithm/contains.hpp>
#include <range/v3/algorithm/copy.hpp>

#include "orm/databaseconnection.h"
#include "orm/ormerror.h"
#include "orm/query/querybuilder.h"
#include "orm/tiny/relations/belongsto.h"
#include "orm/tiny/relations/hasone.h"
#include "orm/tiny/relations/hasmany.h"
#include "orm/tiny/tinybuilder.h"
#include "orm/utils/string.h"
#include "orm/utils/type.h"

#ifdef MANGO_COMMON_NAMESPACE
namespace MANGO_COMMON_NAMESPACE
{
#endif
namespace Orm
{

    class DatabaseConnection;
namespace Query
{
    class Builder;
}

namespace Tiny
{
    // TODO decide/unify when to use class/typename keywords for templates silverqx
    template<typename Model>
    class Builder;
    template<typename Model, typename ...AllRelations>
    using TinyBuilder = Builder<Model>;
    using QueryBuilder = Query::Builder;

    // TODO add concept, AllRelations can not contain type defined in "Model" parameter silverqx
    template<typename Model, typename ...AllRelations>
    class BaseModel
    {
    public:
        /*! The "type" of the primary key ID. */
        using KeyType = quint64;

        explicit BaseModel(const QVector<AttributeItem> &attributes = {});

        // TODO inline static method vs constexpr static, check it silverqx
        /*! Begin querying the model. */
        inline static std::unique_ptr<TinyBuilder<Model>> query()
        { return Model().newQuery(); }

        /*! Begin querying a model with eager loading. */
        static std::unique_ptr<TinyBuilder<Model>>
        with(const QVector<WithItem> &relations);
        /*! Begin querying a model with eager loading. */
        inline static std::unique_ptr<TinyBuilder<Model>>
        with(const QString &relation)
        { return with(QVector<WithItem> {{relation}}); }

        /*! Delete the model from the database. */
        bool remove();
        /*! Delete the model from the database. */
        inline bool deleteModel()
        { return remove(); }
        // TODO cpp check all int types and use std::size_t where appropriate silverqx
        // WARNING id should be Model::KeyType, if I don't solve this problem, do runtime type check, QVariant type has to be the same type like KeyType and throw exception silverqx
        /*! Destroy the models for the given IDs. */
        static std::size_t
        destroy(const QVector<QVariant> &ids);
        static inline std::size_t
        destroy(const QVariant id)
        { return destroy({id}); }

        /*! Get a new query builder for the model's table. */
        inline std::unique_ptr<TinyBuilder<Model>> newQuery()
        { return newQueryWithoutScopes(); }
        /*! Get a new query builder that doesn't have any global scopes. */
        std::unique_ptr<TinyBuilder<Model>> newQueryWithoutScopes();
        /*! Get a new query builder that doesn't have any global scopes or eager loading. */
        std::unique_ptr<TinyBuilder<Model>> newModelQuery();
        /*! Create a new Tiny query builder for the model. */
        std::unique_ptr<TinyBuilder<Model>>
        newTinyBuilder(const QSharedPointer<QueryBuilder> query);

        /*! Create a new model instance that is existing. */
        Model newFromBuilder(const QVector<AttributeItem> &attributes = {},
                             const std::optional<QString> connection = std::nullopt);
        /*! Create a new instance of the given model. */
        Model newInstance(const QVector<AttributeItem> &attributes = {},
                          bool exists = false);

        /*! Static cast this to a child's instance type instance (CRTP). */
        inline Model &model()
        { return static_cast<Model &>(*this); }
        /*! Static cast this to a child's instance type (CRTP), const version. */
        inline const Model &model() const
        { return static_cast<const Model &>(*this); }

        /*! Get the current connection name for the model. */
        inline const QString &getConnectionName() const
        { return model().u_connection; }
        /*! Get the database connection for the model. */
        inline DatabaseConnection &getConnection() const
        { return DatabaseConnection::instance(); }
        /*! Set the connection associated with the model. */
        inline Model &setConnection(const QString &name)
        { model().u_connection = name; return model(); }
        /*! Set the table associated with the model. */
        inline Model &setTable(const QString &value)
        { model().u_table = value; return model(); }
        /*! Get the table associated with the model. */
        inline const QString &getTable() const
        { return model().u_table; }
        /*! Get the primary key for the model. */
        inline const QString &getKeyName() const
        { return model().u_primaryKey; }
        /*! Get the value of the model's primary key. */
        inline QVariant getKey() const
        { return getAttribute(getKeyName()); }

        /*! Fill the model with an array of attributes. */
        Model &fill(const QVector<AttributeItem> &attributes);

        /*! Indicates if the model exists. */
        bool exists = false;

        /* HasAttributes */
        /*! Set a given attribute on the model. */
        Model &setAttribute(const QString &key, const QVariant &value);
        /*! Set the array of model attributes. No checking is done. */
        Model &setRawAttributes(const QVector<AttributeItem> &attributes,
                                bool sync = false);
        /*! Sync the original attributes with the current. */
        Model &syncOriginal();
        /*! Get all of the current attributes on the model. */
        const QVector<AttributeItem> &getAttributes() const;
        /*! Get an attribute from the model. */
        QVariant getAttribute(const QString &key) const;
        /*! Get a plain attribute (not a relationship). */
        QVariant getAttributeValue(const QString &key) const;
        /*! Get an attribute from the $attributes array. */
        QVariant getAttributeFromArray(const QString &key) const;
        /*! Transform a raw model value using mutators, casts, etc. */
        QVariant transformModelValue(const QString &key, const QVariant &value) const;
        /*! Get a relationship for Many types relation. */
        template<class Related,
                 template<typename> typename Container = QVector>
        Container<Related>
        getRelationValue(const QString &relation);
        /*! Get a relationship for a One type relation. */
        template<typename Related, typename Tag,
                 std::enable_if_t<std::is_same_v<Tag, One>, bool> = true>
        std::optional<Related>
        getRelationValue(const QString &relation);

        /* HasRelationships */
        // TODO make getRelation() Container argument compatible with STL containers API silverqx
        /*! Get a specified relationship. */
        template<typename Related, template<typename> typename Container = QVector>
        Container<Related> getRelation(const QString &name);
        /*! Get a specified relationship as Related type, for use with HasOne and BelongsTo relation types. */
        template<typename Related, typename Tag,
                 std::enable_if_t<std::is_same_v<Tag, One>, bool> = true>
        std::optional<Related> getRelation(const QString &name);
        /*! Determine if the given relation is loaded. */
        inline bool relationLoaded(const QString &relation) const
        { return m_relations.contains(relation); };
        /*! Set the given relationship on the model. */
        template<typename Related>
        Model &setRelation(const QString &relation, const QVector<Related> &models);
        /*! Set the given relationship on the model. */
        template<typename Related>
        Model &setRelation(const QString &relation, QVector<Related> &&models);
        /*! Set the given relationship on the model. */
        template<typename Related>
        Model &setRelation(const QString &relation, const std::optional<Related> &model);
        /*! Set the given relationship on the model. */
        template<typename Related>
        Model &setRelation(const QString &relation, std::optional<Related> &&model);
        /*! Get the default foreign key name for the model. */
        QString getForeignKey() const;
        /*! Define a one-to-one relationship. */
        template<typename Related>
        std::unique_ptr<Relations::Relation<Model, Related>>
        hasOne(QString foreignKey = "", QString localKey = "");
        /*! Define an inverse one-to-one or many relationship. */
        template<typename Related>
        std::unique_ptr<Relations::Relation<Model, Related>>
        belongsTo(QString foreignKey = "", QString ownerKey = "", QString relation = "");
        /*! Define a one-to-many relationship. */
        template<typename Related>
        std::unique_ptr<Relations::Relation<Model, Related>>
        hasMany(QString foreignKey = "", QString localKey = "");

        /*! Invoke Model::eagerVisitor() to define template argument Related for eagerLoaded relation. */
        void eagerLoadRelationVisitor(const WithItem &relation, TinyBuilder<Model> &builder,
                                      QVector<Model> &models);
        /*! Get a relation method in the relations hash field defined in the current model instance. */
        const std::any &getRelationMethod(const QString &name) const;

    protected:
        /*! Get a new query builder instance for the connection. */
        QSharedPointer<QueryBuilder> newBaseQueryBuilder() const;

        /*! Continue execution after a relation type was obtained ( by Related template parameter ). */
        template<typename Related>
        inline void eagerVisited()
        { m_eagerStore->builder.template eagerLoadRelation<Related>(
                        m_eagerStore->models, m_eagerStore->relation); }

        /* HasAttributes */
        /*! Get a relationship value from a method. */
        // TODO I think this can be merged to one template method, I want to preserve Orm::One/Many tags and use std::enable_if to switch types by Orm::One/Many tag 🤔 silverqx
        template<class Related, typename Tag,
                 std::enable_if_t<std::is_same_v<Tag, Many>, bool> = true>
        QVector<Related>
        getRelationshipFromMethod(const QString &relation);
        /*! Get a relationship value from a method. */
        template<class Related, typename Tag,
                 std::enable_if_t<std::is_same_v<Tag, One>, bool> = true>
        std::optional<Related>
        getRelationshipFromMethod(const QString &relation);

        /* HasRelationships */
        /*! Create a new model instance for a related model. */
        template<typename Related>
        Related newRelatedInstance() const;
        // TODO can be unified to a one templated method by relation type silverqx
        /*! Instantiate a new HasOne relationship. */
        template<typename Related>
        inline std::unique_ptr<Relations::Relation<Model, Related>>
        newHasOne(std::unique_ptr<TinyBuilder<Related>> &&query, Model &parent,
                  const QString &foreignKey, const QString &localKey) const
        { return Relations::HasOne<Model, Related>::create(
                        std::move(query), parent, foreignKey, localKey); }
        /*! Instantiate a new BelongsTo relationship. */
        template<typename Related>
        inline std::unique_ptr<Relations::Relation<Model, Related>>
        newBelongsTo(std::unique_ptr<TinyBuilder<Related>> &&query,
                     Model &child, const QString &foreignKey,
                     const QString &ownerKey, const QString &relation) const
        { return Relations::BelongsTo<Model, Related>::create(
                        std::move(query), child, foreignKey, ownerKey, relation); }
        /*! Instantiate a new HasMany relationship. */
        template<typename Related>
        inline std::unique_ptr<Relations::Relation<Model, Related>>
        newHasMany(std::unique_ptr<TinyBuilder<Related>> &&query, Model &parent,
                   const QString &foreignKey, const QString &localKey) const
        { return Relations::HasMany<Model, Related>::create(
                        std::move(query), parent, foreignKey, localKey); }
        /*! Guess the "belongs to" relationship name. */
        template<typename Related>
        QString guessBelongsToRelation() const;

        /*! Perform the actual delete query on this model instance. */
        void performDeleteOnModel();

        /*! Set the keys for a save update query. */
        TinyBuilder<Model> &
        setKeysForSaveQuery(TinyBuilder<Model> &query);
        /*! Get the primary key value for a save query. */
        QVariant getKeyForSaveQuery() const;

        /*! The connection name for the model. */
        QString u_connection {""};
        /*! The primary key for the model. */
        QString u_primaryKey {"id"};

        // TODO for std::any check, whether is appropriate to define template requirement std::is_nothrow_move_constructible ( to avoid dynamic allocations for small objects and how this internally works ) silverqx
        /*! Map of relation names to methods. */
        QHash<QString, std::any> u_relations;
        /*! The relations to eager load on every query. */
        QVector<WithItem> u_with;
        /*! The relationship counts that should be eager loaded on every query. */
        QVector<WithItem> u_withCount;

        /* HasAttributes */
        // TODO should be QHash, I choosen QVector, becuase I wanted to preserve attributes order, think about this, would be solution to use undered_map which preserves insert order? and do I really need to preserve insert order? 🤔, the same is true for m_original field silverqx
        /*! The model's attributes. */
        QVector<AttributeItem> m_attributes;
        /*! The model attribute's original state. */
        QVector<AttributeItem> m_original;

        /* HasRelationships */
        /*! The loaded relationships for the model. */
        QHash<QString,
        std::variant<QVector<AllRelations>..., std::optional<AllRelations>...>> m_relations;

    private:
        /* HasRelationships */
        /*! Throw exception if a relation is not defined. */
        void validateUserRelation(const QString &name) const;

        /*! Helps to avoid passing variables to the Model::eagerVisitor(). */
        struct EagerRelationStoreItem
        {
            const WithItem     &relation;
            TinyBuilder<Model> &builder;
            QVector<Model>     &models;
        };
        /*! Store to save values before Model::eagerVisitor() is called. */
        const EagerRelationStoreItem *m_eagerStore = nullptr;
    };

    template<typename Model, typename ...AllRelations>
    BaseModel<Model, AllRelations...>::BaseModel(const QVector<AttributeItem> &attributes)
    {
        // Compile time check if a primary key type is supported by a QVariant
        qMetaTypeId<typename Model::KeyType>();

        syncOriginal();

        fill(attributes);
    }

    template<typename Model, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Model>>
    BaseModel<Model, AllRelations...>::with(const QVector<WithItem> &relations)
    {
        auto builder = query();

        builder->with(relations);

        return builder;
    }

    template<typename Model, typename ...AllRelations>
    bool BaseModel<Model, AllRelations...>::remove()
    {
        // TODO future add support for attributes casting silverqx
//        $this->mergeAttributesFromClassCasts();

        if (getKeyName().isEmpty())
            throw OrmError("No primary key defined on model.");

        /* If the model doesn't exist, there is nothing to delete so we'll just return
           immediately and not do anything else. Otherwise, we will continue with a
           deletion process on the model, firing the proper events, and so forth. */
        if (!exists)
            // TODO api different silverqx
            return false;

        // TODO future add support for model events silverqx
//        if ($this->fireModelEvent('deleting') === false) {
//            return false;
//        }

        // TODO add support for model timestamps silverqx
        /* Here, we'll touch the owning models, verifying these timestamps get updated
           for the models. This will allow any caching to get broken on the parents
           by the timestamp. Then we will go ahead and delete the model instance. */
//        $this->touchOwners();

        // TODO performDeleteOnModel() and return value, check logic here, eg what happens when no model is delete and combinations silverqx
        performDeleteOnModel();

        /* Once the model has been deleted, we will fire off the deleted event so that
           the developers may hook into post-delete operations. We will then return
           a boolean true as the delete is presumably successful on the database. */
//        $this->fireModelEvent('deleted', false);

        return true;
    }

    // TODO next test all this remove()/destroy() methods, when deletion fails silverqx
    template<typename Model, typename ...AllRelations>
    size_t
    BaseModel<Model, AllRelations...>::destroy(const QVector<QVariant> &ids)
    {
        if (ids.isEmpty())
            return 0;

        /* We will actually pull the models from the database table and call delete on
           each of them individually so that their events get fired properly with a
           correct set of attributes in case the developers wants to check these. */
        Model instance;
        const auto &key = instance.getKeyName();

        std::size_t count = 0;

        // TODO diff call whereIn() on Model vs TinyBuilder silverqx
        // Ownership of a unique_ptr()
        for (auto &model : instance.newQuery()->whereIn(key, ids).get())
            if (model.remove())
                ++count;

        return count;
    }

    template<typename Model, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Model>>
    BaseModel<Model, AllRelations...>::newQueryWithoutScopes()
    {
        auto tinyBuilder = newModelQuery();

        tinyBuilder->with(model().u_with);

        return tinyBuilder;
    }

    template<typename Model, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Model>>
    BaseModel<Model, AllRelations...>::newModelQuery()
    {
        return newTinyBuilder(newBaseQueryBuilder());
    }

    template<typename Model, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Model>>
    BaseModel<Model, AllRelations...>::newTinyBuilder(const QSharedPointer<QueryBuilder> query)
    {
        return std::make_unique<TinyBuilder<Model>>(query, model());
    }

    template<typename Model, typename ...AllRelations>
    Model
    BaseModel<Model, AllRelations...>::newFromBuilder(const QVector<AttributeItem> &attributes,
                                                      const std::optional<QString> connection)
    {
        auto model = newInstance({}, true);

        model.setRawAttributes(attributes, true);

        model.setConnection(connection ? *connection : getConnectionName());

        return model;
    }

    template<typename Model, typename ...AllRelations>
    Model
    BaseModel<Model, AllRelations...>::newInstance(const QVector<AttributeItem> &attributes,
                                                   const bool exists)
    {
        /* This method just provides a convenient way for us to generate fresh model
           instances of this current model. It is particularly useful during the
           hydration of new objects via the Eloquent query builder instances. */
        Model model(attributes);

        model.exists = exists;
        model.setConnection(getConnectionName());
        model.setTable(getTable());

        return model;
    }

    template<typename Model, typename ...AllRelations>
    QSharedPointer<QueryBuilder>
    BaseModel<Model, AllRelations...>::newBaseQueryBuilder() const
    {
        return getConnection().queryBuilder();
    }

    template<typename Model, typename ...AllRelations>
    template<class Related, template<typename> typename Container>
    Container<Related>
    BaseModel<Model, AllRelations...>::getRelationValue(const QString &relation)
    {
        /*! If the key already exists in the relationships array, it just means the
            relationship has already been loaded, so we'll just return it out of
            here because there is no need to query within the relations twice. */
        if (relationLoaded(relation)) {
            // TODO duplicate silverqx
            Container<Related> relatedModels;

            for (auto &v : std::get<QVector<Related>>(m_relations.find(relation).value()))
                relatedModels.push_back(v);

            return relatedModels;
        }

        /*! If the "attribute" exists as a method on the model, we will just assume
            it is a relationship and will load and return results from the query
            and hydrate the relationship's value on the "relationships" array. */
//        if (method_exists($this, $key) ||
//            (static::$relationResolvers[get_class($this)][$key] ?? null)) {
//            return $this->getRelationshipFromMethod($key);
//        }
        // Check, if this relation is defined
        if (model().u_relations.contains(relation))
            return getRelationshipFromMethod<Related, Many>(relation);

        return {};
    }

    template<typename Model, typename ...AllRelations>
    template<typename Related, typename Tag,
             std::enable_if_t<std::is_same_v<Tag, One>, bool>>
    std::optional<Related>
    BaseModel<Model, AllRelations...>::getRelationValue(const QString &relation)
    {
        /*! If the key already exists in the relationships array, it just means the
            relationship has already been loaded, so we'll just return it out of
            here because there is no need to query within the relations twice. */
        if (relationLoaded(relation))
            return std::get<std::optional<Related>>(m_relations.find(relation).value());

        // Check, if a relation is defined and if it is, get it
        if (model().u_relations.contains(relation))
            return getRelationshipFromMethod<Related, One>(relation);

        return std::nullopt;
    }

    template<typename Model, typename ...AllRelations>
    template<class Related, typename Tag,
             std::enable_if_t<std::is_same_v<Tag, Many>, bool>>
    QVector<Related>
    BaseModel<Model, AllRelations...>::getRelationshipFromMethod(const QString &relationName)
    {
        // Obtain related model
        auto relatedModel = std::get<QVector<Related>>(
                std::invoke(
                    std::any_cast<RelationType<Model, Related>>(model().u_relations[relationName]),
                    model())
                ->getResults());

        setRelation(relationName, relatedModel);

        return relatedModel;
    }

    template<typename Model, typename ...AllRelations>
    template<class Related, typename Tag,
             std::enable_if_t<std::is_same_v<Tag, One>, bool>>
    std::optional<Related>
    BaseModel<Model, AllRelations...>::getRelationshipFromMethod(const QString &relationName)
    {
        // Obtain related model
        auto relatedModel = std::get<std::optional<Related>>(
                std::invoke(
                    std::any_cast<RelationType<Model, Related>>(model().u_relations[relationName]),
                    model())
                ->getResults());

        setRelation(relationName, relatedModel);

        return relatedModel;
    }

    // TODO solve different behavior like Eloquent getRelation() silverqx
    // TODO next many relation compiles with Orm::One and exception during runtime occures, solve this during compile, One relation only with Orm::One and many relation type only with Container version silverqx
    template<typename Model, typename ...AllRelations>
    template<typename Related, template<typename> typename Container>
    Container<Related>
    BaseModel<Model, AllRelations...>::getRelation(const QString &name)
    {
        if (!relationLoaded(name))
            // TODO create RelationError class silverqx
            throw OrmError("Undefined relation key (in m_relations) : " + name);

        // TODO make it vector or pointers/references to models (relations), may be not needed to modify models and call save()/push() silverqx
        Container<Related> relatedModels;
        ranges::copy(std::get<QVector<Related>>(m_relations.find(name).value()),
                     ranges::back_inserter(relatedModels));

        return relatedModels;
    }

    template<typename Model, typename ...AllRelations>
    template<typename Related, typename Tag,
             std::enable_if_t<std::is_same_v<Tag, One>, bool>>
    std::optional<Related>
    BaseModel<Model, AllRelations...>::getRelation(const QString &name)
    {
        // TODO duplicated if statement silverqx
        if (!relationLoaded(name))
            // TODO create RelationError class silverqx
            throw OrmError("Undefined relation key (in m_relations) : " + name);

        // TODO instantiate relation by name and check if is_base_of OneRelation/ManyRelation, to have nice exception message (in debug mode only), because is impossible to check this during compile time silverqx
        // TODO should I return references to m_relations or copies? decide when I will implement save/push to synchronize changed attributes back to db silverqx
        return std::get<std::optional<Related>>(m_relations.find(name).value());
    }

    template<typename Model, typename ...AllRelations>
    template<typename Related>
    Model &
    BaseModel<Model, AllRelations...>::setRelation(const QString &relation,
                                                   const QVector<Related> &models)
    {
        m_relations.insert(relation, models);

        return model();
    }

    template<typename Model, typename ...AllRelations>
    template<typename Related>
    Model &
    BaseModel<Model, AllRelations...>::setRelation(const QString &relation,
                                                   QVector<Related> &&models)
    {
        m_relations.insert(relation, std::move(models));

        return model();
    }

    // TODO next unify setRelation() methods silverqx
    template<typename Model, typename ...AllRelations>
    template<typename Related>
    Model &
    BaseModel<Model, AllRelations...>::setRelation(const QString &relation,
                                                   const std::optional<Related> &model)
    {
        m_relations.insert(relation, model);

        return this->model();
    }

    template<typename Model, typename ...AllRelations>
    template<typename Related>
    Model &
    BaseModel<Model, AllRelations...>::setRelation(const QString &relation,
                                                   std::optional<Related> &&model)
    {
        m_relations.insert(relation, std::move(model));

        return this->model();
    }

    template<typename Model, typename ...AllRelations>
    Model &
    BaseModel<Model, AllRelations...>::fill(const QVector<AttributeItem> &attributes)
    {
        for (const auto &attribute : attributes)
            setAttribute(attribute.key, attribute.value);

        return model();
    }

    template<typename Model, typename ...AllRelations>
    Model &BaseModel<Model, AllRelations...>::setAttribute(
            const QString &key, const QVariant &value)
    {
        m_attributes.append({key, value});

        return model();
    }

    template<typename Model, typename ...AllRelations>
    Model &
    BaseModel<Model, AllRelations...>::setRawAttributes(
            const QVector<AttributeItem> &attributes,
            const bool sync)
    {
        m_attributes = attributes;

        if (sync)
            syncOriginal();

        return model();
    }

    template<typename Model, typename ...AllRelations>
    Model &BaseModel<Model, AllRelations...>::syncOriginal()
    {
        m_original = getAttributes();

        return model();
    }

    template<typename Model, typename ...AllRelations>
    const QVector<AttributeItem> &
    BaseModel<Model, AllRelations...>::getAttributes() const
    {
        /*mergeAttributesFromClassCasts();*/
        return m_attributes;
    }

    template<typename Model, typename ...AllRelations>
    QVariant BaseModel<Model, AllRelations...>::getAttribute(const QString &key) const
    {
        if (key.isEmpty() || key.isNull())
            return {};

        const auto containsKey = ranges::contains(m_attributes, true,
                                                  [&key](const auto &attribute)
        {
            return attribute.key == key;
        });

        /* If the attribute exists in the attribute array or has a "get" mutator we will
           get the attribute's value. Otherwise, we will proceed as if the developers
           are asking for a relationship's value. This covers both types of values. */
        if (containsKey
//            || array_key_exists($key, $this->casts)
//            || hasGetMutator(key)
//            || isClassCastable(key)
        )
            return getAttributeValue(key);

        return {};
        // TODO Eloquent returns relation when didn't find attribute, decide how to solve this, or add NOTE about different api silverqx
//        return $this->getRelationValue($key);
    }

    template<typename Model, typename ...AllRelations>
    QVariant BaseModel<Model, AllRelations...>::getAttributeValue(const QString &key) const
    {
        return transformModelValue(key, getAttributeFromArray(key));
    }

    // TODO candidate for optional const reference, to be able return null value and use reference at the same time silverqx
    template<typename Model, typename ...AllRelations>
    QVariant BaseModel<Model, AllRelations...>::getAttributeFromArray(const QString &key) const
    {
        const auto &attributes = getAttributes();
        const auto itAttribute = ranges::find(attributes, true,
                                              [&key](const auto &attribute)
        {
            return attribute.key == key;
        });

        // Not found
        if (itAttribute == ranges::end(attributes))
            return {};

        return itAttribute->value;
    }

    template<typename Model, typename ...AllRelations>
    QVariant BaseModel<Model, AllRelations...>::transformModelValue(
            const QString &key,
            const QVariant &value) const
    {
        Q_UNUSED(key)

        return value;
    }

    template<typename Model, typename ...AllRelations>
    QString BaseModel<Model, AllRelations...>::getForeignKey() const
    {
        return QStringLiteral("%1_%2").arg(
                    Utils::String::toSnake(Utils::Type::classPureBasename<decltype (model())>()),
                    getKeyName());
    }

    template<typename Model, typename ...AllRelations>
    template<typename Related>
    Related BaseModel<Model, AllRelations...>::newRelatedInstance() const
    {
        Related instance;

        if (instance.getConnectionName().isEmpty())
            instance.setConnection(getConnectionName());

        return instance;
    }

    template<typename Model, typename ...AllRelations>
    template<typename Related>
    std::unique_ptr<Relations::Relation<Model, Related>>
    BaseModel<Model, AllRelations...>::hasOne(QString foreignKey, QString localKey)
    {
        auto instance = newRelatedInstance<Related>();

        if (foreignKey.isEmpty())
            foreignKey = getForeignKey();

        if (localKey.isEmpty())
            localKey = getKeyName();

        return newHasOne<Related>(instance.newQuery(), model(),
                                  instance.getTable() + '.' + foreignKey, localKey);
    }

    template<typename Model, typename ...AllRelations>
    template<typename Related>
    std::unique_ptr<Relations::Relation<Model, Related>>
    BaseModel<Model, AllRelations...>::belongsTo(QString foreignKey, QString ownerKey,
                                                 QString relation)
    {
        /* If no relation name was given, we will use the Related class type to extract
           the name and use that as the relationship name as most of the time this
           will be what we desire to use for the relationships. */
        if (relation.isEmpty())
            relation = guessBelongsToRelation<Related>();

        auto instance = newRelatedInstance<Related>();

        const auto &primaryKey = instance.getKeyName();

        /* If no foreign key was supplied, we can use a backtrace to guess the proper
           foreign key name by using the name of the relationship function, which
           when combined with an "_id" should conventionally match the columns. */
        if (foreignKey.isEmpty())
            foreignKey = Utils::String::toSnake(relation) + '_' + primaryKey;

        /* Once we have the foreign key names, we'll just create a new Eloquent query
           for the related models and returns the relationship instance which will
           actually be responsible for retrieving and hydrating every relations. */
        if (ownerKey.isEmpty())
            ownerKey = primaryKey;

        return newBelongsTo<Related>(instance.newQuery(), model(),
                                     foreignKey, ownerKey, relation);
    }

    template<typename Model, typename ...AllRelations>
    template<typename Related>
    std::unique_ptr<Relations::Relation<Model, Related>>
    BaseModel<Model, AllRelations...>::hasMany(QString foreignKey, QString localKey)
    {
        auto instance = newRelatedInstance<Related>();

        if (foreignKey.isEmpty())
            foreignKey = getForeignKey();

        if (localKey.isEmpty())
            localKey = getKeyName();

        return newHasMany<Related>(instance.newQuery(), model(),
                                   instance.getTable() + '.' + foreignKey, localKey);
    }

    template<typename Model, typename ...AllRelations>
    template<typename Related>
    QString BaseModel<Model, AllRelations...>::guessBelongsToRelation() const
    {
        auto relation = Utils::Type::classPureBasename<Related>();

        relation[0] = relation[0].toLower();

        return relation;
    }

    template<typename Model, typename ...AllRelations>
    void BaseModel<Model, AllRelations...>::eagerLoadRelationVisitor(
            const WithItem &relation, TinyBuilder<Model> &builder, QVector<Model> &models)
    {
        // Throw excpetion if a relation is not defined
        validateUserRelation(relation.name);

        EagerRelationStoreItem eagerStore {relation, builder, models};
        m_eagerStore = &eagerStore;

        model().eagerVisitor(relation.name);
    }

    template<typename Model, typename ...AllRelations>
    const std::any &
    BaseModel<Model, AllRelations...>::getRelationMethod(const QString &name) const
    {
        // Throw excpetion if a relation is not defined
        validateUserRelation(name);

        return model().u_relations.find(name).value();
    }

    template<typename Model, typename ...AllRelations>
    void BaseModel<Model, AllRelations...>::validateUserRelation(const QString &name) const
    {
        if (!model().u_relations.contains(name))
            throw OrmError("Undefined relation key (in relations) : " + name);
    }

    template<typename Model, typename ...AllRelations>
    void BaseModel<Model, AllRelations...>::performDeleteOnModel()
    {
        // TODO ask eg on stackoverflow, if I have to save unique_ptr to local variable or pass it right away down silverqx
        // Ownership of a unique_ptr()
        setKeysForSaveQuery(*newModelQuery()).remove();

        this->exists = false;
    }

    template<typename Model, typename ...AllRelations>
    TinyBuilder<Model> &
    BaseModel<Model, AllRelations...>::setKeysForSaveQuery(TinyBuilder<Model> &query)
    {
        return query.where(getKeyName(), QStringLiteral("="), getKeyForSaveQuery());
    }

    template<typename Model, typename ...AllRelations>
    QVariant BaseModel<Model, AllRelations...>::getKeyForSaveQuery() const
    {
        // TODO reason, why m_attributes and m_original should be QHash silverqx
        const auto itOriginal = ranges::find(m_original, true,
                                             [&key = getKeyName()](const auto &original)
        {
            return original.key == key;
        });

        return itOriginal != ranges::end(m_original) ? itOriginal->value : getKey();
    }

} // namespace Orm::Tiny
} // namespace Orm
#ifdef MANGO_COMMON_NAMESPACE
} // namespace MANGO_COMMON_NAMESPACE
#endif

#endif // BASEMODEL_H
