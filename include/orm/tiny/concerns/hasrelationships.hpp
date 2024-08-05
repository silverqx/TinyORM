#pragma once
#ifndef ORM_TINY_CONCERNS_HASRELATIONSHIPS_HPP
#define ORM_TINY_CONCERNS_HASRELATIONSHIPS_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <range/v3/algorithm/contains.hpp>

#include "orm/exceptions/invalidtemplateargumenterror.hpp"
#include "orm/tiny/concerns/hasrelationstore.hpp"
#include "orm/tiny/exceptions/relationmappingnotfounderror.hpp"
#include "orm/tiny/exceptions/relationnotloadederror.hpp"
#include "orm/tiny/macros/crtpmodelwithbase.hpp"
#include "orm/tiny/relations/belongsto.hpp"
#include "orm/tiny/relations/belongstomany.hpp"
#include "orm/tiny/relations/hasmany.hpp"
#include "orm/tiny/relations/hasone.hpp"
#include "orm/utils/string.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny
{
namespace Relations
{
    class IsPivotModel;
}
namespace Concerns
{

// Clang 18 crashes with this concept, requires doesn't work all well
#ifndef __clang__
    /*! RelationsContainer<AllRelations...> container concept. */
    template<typename C, typename ...AllRelations>
    concept RelationsContainerConcept =
            std::convertible_to<C, RelationsContainer<AllRelations...>>;
#endif

    /*! Model relationships. */
    template<typename Derived, AllRelationsConcept ...AllRelations>
    class HasRelationships : // NOLINT(bugprone-exception-escape, misc-no-recursion)
            private Concerns::HasRelationStore<Derived, AllRelations...>
    {
        /* Using starts in the BaseRelationStore::visit() and is used to access private
           visited methods. */
        friend class Concerns::HasRelationStore<Derived, AllRelations...>;
        // To access private queriesRelationshipsWithVisitor()
        friend class Concerns::QueriesRelationships<Derived>;
        // To access XyzVisitor()-s, replaceRelations() and few other private methods
        friend class Model<Derived, AllRelations...>;
        // To access private pushVisited()
        friend class Support::Stores::PushRelationStore<Derived, AllRelations...>;
        // To access private touchOwnersVisited()
        friend class Support::Stores::TouchOwnersRelationStore<Derived, AllRelations...>;
        // To access private serializeRelationVisited()
        template<SerializedAttributes C, typename Derived_,
                 AllRelationsConcept ...AllRelations_>
        friend class Support::Stores::SerializeRelationStore; // Partial specialization doens't work with friend
        // To access eagerLoadRelationWithVisitor()
        friend class Tiny::Builder<Derived>;

        /*! Alias for the attribute utils. */
        using AttributeUtils = Orm::Tiny::Utils::Attribute;
        /*! Alias for the string utils. */
        using StringUtils = Orm::Utils::String;
        /*! Alias for the type utils. */
        using TypeUtils = Orm::Utils::Type;

    public:
        /*! Alias for the RelationsContainer. */
        using RelationsContainerType = RelationsContainer<AllRelations...>;

        /* HasRelationships related */
        /*! Get a relationship for Many types relation (load a relationship lazily if
            not loaded). */
        template<typename Related,
                 template<typename> typename Container = ModelsCollection>
        const Container<Related *>
        getRelationValue(const QString &relation);
        /*! Get a relationship for a One type relation (load a relationship lazily if
            not loaded). */
        template<typename Related, typename Tag> requires std::same_as<Tag, One>
        Related *
        getRelationValue(const QString &relation);

        /*! Get a specified relationship (throw if a relationship is not loaded). */
        template<typename Related,
                 template<typename> typename Container = ModelsCollection>
        const Container<Related *>
        getRelation(const QString &relation);
        /*! Get a specified relationship as Related type, for use with HasOne and
            BelongsTo relation types (throw if a relationship is not loaded). */
        template<typename Related, typename Tag> requires std::same_as<Tag, One>
        Related *getRelation(const QString &relation);

        /*! Determine if the given relation is loaded. */
        bool relationLoaded(const QString &relation) const;

        /*! Set the given relationship on the model. */
        template<typename Related>
        Derived &setRelation(const QString &relation,
                             const ModelsCollection<Related> &models);
        /*! Set the given relationship on the model. */
        template<typename Related>
        Derived &setRelation(const QString &relation,
                             ModelsCollection<Related> &&models);

        /*! Set the given relationship on the model. */
        template<typename Related>
        Derived &setRelation(const QString &relation,
                             const std::optional<Related> &model);
        /*! Set the given relationship on the model. */
        template<typename Related>
        Derived &setRelation(const QString &relation, std::optional<Related> &&model);

        /*! Set the given relationship on the model. */
        template<typename Related>
        requires std::is_base_of_v<Relations::IsPivotModel, Related>
        Derived &setRelation(const QString &relation,
                             const std::optional<Related> &model);
        /*! Set the given relationship on the model. */
        template<typename Related>
        requires std::is_base_of_v<Relations::IsPivotModel, Related>
        Derived &setRelation(const QString &relation, std::optional<Related> &&model);

        /*! Get all the loaded relations for the instance. */
        inline const RelationsContainer<AllRelations...> &getRelations() const noexcept;
        /*! Get all the loaded relations for the instance. */
        inline RelationsContainer<AllRelations...> &getRelations() noexcept;

        /*! Unset all the loaded relations for the instance. */
        Derived &unsetRelations();
        /*! Unset a loaded relationship. */
        Derived &unsetRelation(const QString &relation);

        /* Relationships factory methods */
        /*! Define a one-to-one relationship. */
        template<typename Related>
        std::unique_ptr<Relations::HasOne<Derived, Related>>
        hasOne(QString foreignKey = "", QString localKey = "");
        /*! Define an inverse one-to-one or many relationship. */
        template<typename Related>
        std::unique_ptr<Relations::BelongsTo<Derived, Related>>
        belongsTo(QString foreignKey = "", QString ownerKey = "", QString relation = "");
        /*! Define a one-to-many relationship. */
        template<typename Related>
        std::unique_ptr<Relations::HasMany<Derived, Related>>
        hasMany(QString foreignKey = "", QString localKey = "");
        /*! Define a many-to-many relationship. */
        template<typename Related, typename PivotType = Relations::Pivot>
        std::unique_ptr<Relations::BelongsToMany<Derived, Related, PivotType>>
        belongsToMany(QString table = "", QString foreignPivotKey = "",
                      QString relatedPivotKey = "", QString parentKey = "",
                      QString relatedKey = "", QString relation = "");

        /* Touching timestamps */
        /*! Touch the owning relations of the model. */
        void touchOwners() const;

        /*! Get the relationships that are touched on save. */
        inline const QStringList &getTouchedRelations() const;

        /*! Add one relationship that is touched on save. */
        inline Derived &addTouch(const QString &touch);
        /*! Add one relationship that is touched on save. */
        inline Derived &addTouch(QString &&touch);

        /*! Add one relationship that is touched on save. */
        inline Derived &addTouches(const QStringList &touches);
        /*! Add one relationship that is touched on save. */
        inline Derived &addTouches(QStringList &&touches);

        /*! Set the relationships that are touched on save. */
        inline Derived &setTouchedRelations(const QStringList &touches);
        /*! Set the relationships that are touched on save. */
        inline Derived &setTouchedRelations(QStringList &&touches);

        /*! Determine if the model touches a given relation. */
        inline bool touches(const QString &relation) const;

        /*! Clear the u_touches for the model. */
        inline Derived &clearTouches();

        /* Serialization - Relations */
        /*! Convert the model's relationships to the map or vector. */
        template<SerializedAttributes C, typename PivotType = void>
        C serializeRelations() const;

        /* Others */
        /*! Equality comparison operator for the HasRelationships concern. */
        bool operator==(const HasRelationships &right) const;

        /*! Determine whether the current model contains a pivot relation alternative
            in the m_relations std::variant. */
        constexpr static bool hasPivotRelation() noexcept;

    protected:
        /*! Relation visitor lambda type. */
        using RelationVisitor = std::function<void(
                typename Concerns::HasRelationStore<Derived, AllRelations...>
                                 ::BaseRelationStore &)>;

        /*! Get a relationship value from a method. */
        template<class Related,
                 template<typename> typename Container = ModelsCollection>
        Container<Related *>
        getRelationshipFromMethod(const QString &relation);
        /*! Get a relationship value from a method. */
        template<class Related, typename Tag> requires std::same_as<Tag, One>
        Related *
        getRelationshipFromMethod(const QString &relation);

        /*! Set the entire relations hash on the model. */
        Derived &setRelations(const RelationsContainer<AllRelations...> &relations);
        /*! Set the entire relations hash on the model. */
        Derived &setRelations(RelationsContainer<AllRelations...> &&relations);

        /* Relationships factory methods */
        /*! Instantiate a new HasOne relationship. */
        template<typename Related>
        inline std::unique_ptr<Relations::HasOne<Derived, Related>>
        newHasOne(std::unique_ptr<Related> &&related, Derived &parent,
                  const QString &foreignKey, const QString &localKey) const;
        /*! Instantiate a new BelongsTo relationship. */
        template<typename Related>
        inline std::unique_ptr<Relations::BelongsTo<Derived, Related>>
        newBelongsTo(std::unique_ptr<Related> &&related,
                     Derived &child, const QString &foreignKey,
                     const QString &ownerKey, const QString &relation) const;
        /*! Instantiate a new HasMany relationship. */
        template<typename Related>
        inline std::unique_ptr<Relations::HasMany<Derived, Related>>
        newHasMany(std::unique_ptr<Related> &&related, Derived &parent,
                   const QString &foreignKey, const QString &localKey) const;
        /*! Instantiate a new BelongsToMany relationship. */
        template<typename Related, typename PivotType>
        inline std::unique_ptr<Relations::BelongsToMany<Derived, Related, PivotType>>
        newBelongsToMany(std::unique_ptr<Related> &&related, Derived &parent,
                         const QString &table, const QString &foreignPivotKey,
                         const QString &relatedPivotKey, const QString &parentKey,
                         const QString &relatedKey, const QString &relation) const;

        /*! Create a new model instance for a related model. */
        template<typename Related>
        std::unique_ptr<Related> newRelatedInstance() const;

        /*! Guess the "belongs to" relationship name. */
        template<typename Related>
        QString guessBelongsToRelation() const;
        /*! Guess the "belongs to many" relationship name. */
        template<typename Related>
        QString guessBelongsToManyRelation() const;
        /*! Get the joining table name for a many-to-many relation. */
        template<typename Related>
        QString pivotTableName() const;

        /* Serialization - Relations */
        /*! Get a map of all serializable relations (visible/hidden). */
        RelationsContainer<AllRelations...> getSerializableRelations() const;

        /* Others */
        /*! Compare the u_relations hash (size and keys only). */
        static bool compareURelations(
                    const QHash<QString, RelationVisitor> &left,
                    const QHash<QString, RelationVisitor> &right);

        /* Data members */
        /*! Map of relation names to methods. */
        QHash<QString, RelationVisitor> u_relations;

        /* The libstdc++ shipped with the GCC <12.1 doesn't allow an incomplete
           mapped_type (value) in the std::unordered_map. */
        /*! The loaded relationships for the model. */
        RelationsContainer<AllRelations...> m_relations;
        /*! The relationships that should be touched on save. */
        QStringList u_touches;
        // CUR1 use sets instead of QStringList where appropriate silverqx
        /*! Currently loaded Pivot relation names. */
        std::unordered_set<QString> m_pivots;

    private:
        /*! Alias for the enum struct RelationMappingNotFoundError::From. */
        using RelationFrom = Tiny::Exceptions::RelationMappingNotFoundError::From;

        /*! Throw an exception if a relation is not defined. */
        void validateUserRelation(const QString &name,
                                  RelationFrom from = RelationFrom::UNDEFINED) const;

        /*! Obtain related models from "relationships" data member hash
            without any checks. */
        template<class Related,
                 template<typename> typename Container = ModelsCollection>
        Container<Related *>
        getRelationFromHash(const QString &relation);
        /*! Obtain related models from "relationships" data member hash
            without any checks. */
        template<class Related, typename Tag> requires std::same_as<Tag, One>
        Related *
        getRelationFromHash(const QString &relation);

        /*! Create lazy store and obtain a relationship from defined method. */
        template<typename Related, typename Result>
        requires (!std::is_reference_v<Result>)
        Result getRelationshipFromMethodWithVisitor(const QString &relation) const;

        /*! Throw exception if correct getRelation/Value() method was not used, to avoid
            std::bad_variant_access. */
        template<typename Result, typename Related, typename T>
        void checkRelationType(const T &relationVariant, const QString &relation,
                               const QString &source) const;

        /*! Guess the relationship name for belongsTo/belongsToMany. */
        template<typename Related>
        QString guessBelongsToRelationInternal() const;

        /* Eager load relation store related */
        /*! Obtain a relationship instance for eager loading. */
        template<SameDerivedCollectionModel<Derived> CollectionModel>
        void eagerLoadRelationWithVisitor(
                const WithItem &relation, const TinyBuilder<Derived> &builder,
                ModelsCollection<CollectionModel> &models) const;

        /* Get related table for belongs-to-many relation store related */
        /*! Get Related model table name if the relation is BelongsToMany, otherwise
            return empty std::optional. */
        std::optional<QString>
        getRelatedTableForBelongsToManyWithVisitor(const QString &relation) const;

        /* Push relation store related */
        /*! Create push store and call push for every model. */
        bool pushWithVisitor(const QString &relation,
                             RelationsType<AllRelations...> &models) const;

        /*! On the base of alternative held by m_relations decide, which
            pushVisited() to execute. */
        template<typename Related>
        void pushVisited() const;
        /*! Push for Many relation types. */
        template<typename Related, typename Tag> requires std::same_as<Tag, Many>
        void pushVisited() const;
        /*! Push for One relation type. */
        template<typename Related, typename Tag> requires std::same_as<Tag, One>
        void pushVisited() const;

        /* Touch owners store related */
        /*! Create 'touch owners relation store' and touch all related models. */
        void touchOwnersWithVisitor(const QString &relation) const;
        /*! On the base of alternative held by m_relations decide, which
            touchOwnersVisited() to execute. */
        template<typename Related, typename Relation>
        void touchOwnersVisited(const Relation &relation, const QString &relationName);

        /* QueriesRelationships store related */
        /*! Create 'QueriesRelationships relation store' to obtain relation instance. */
        template<typename Related = void>
        void queriesRelationshipsWithVisitor(
                const QString &relation, Concerns::QueriesRelationships<Derived> &origin,
                const QString &comparison, qint64 count, const QString &condition,
                const std::function<void(
                        Concerns::QueriesRelationshipsCallback<Related> &)> &callback,
                std::optional<std::reference_wrapper<
                        QStringList>> relations = std::nullopt) const;

        /* Operations on a Model instance */
        /*! Obtain all loaded relation names except pivot relations. */
        QList<WithItem> getLoadedRelationsWithoutPivot() const;

        /*! Replace relations in the m_relation. */
        void replaceRelations(RelationsContainer<AllRelations...> &relations,
                              const QList<WithItem> &onlyRelations);

        /* Serialization - Relations */
        /*! Create and visit the serialize relation store. */
        template<SerializedAttributes C>
        void serializeRelationWithVisitor(
                const QString &relation, const RelationsType<AllRelations...> &models,
                C &attributes) const;

        /*! On the base of alternative held by m_relations decide, which
            serializeRelation() to execute. */
        template<typename Related, SerializedAttributes C, typename PivotType>
        void serializeRelationVisited(
                QString relation, const RelationsType<AllRelations...> &models,
                C &attributes) const;

        /*! Serialize for Many relation types. */
        template<typename Related, bool isMap, typename PivotType>
        inline static void serializeRelation(QVariant &relationSerialized, // don't remove inline
                                             const ModelsCollection<Related> &models);
        /*! Serialize for One relation type. */
        template<typename Related, bool isMap>
        inline static void serializeRelation(QVariant &relationSerialized, // don't remove inline
                                             const std::optional<Related> &model);

        /*! Insert the serialized relation attributes to the final attributes map. */
        inline static void
        insertSerializedRelation(QVariantMap &attributes, const QString &relation,
                                 const QVariant &relationSerialized);
        /*! Emplace the serialized relation attributes to the final attributes vector. */
        inline static void
        insertSerializedRelation(QList<AttributeItem> &attributes, QString &&relation,
                                 QVariant &&relationSerialized);

        /* Serialization - HidesAttributes */
        /*! Get a relations map of visible serializable relations. */
        static RelationsContainer<AllRelations...>
        getSerializableVisibleRelations(
                const RelationsContainer<AllRelations...> &relations,
                const std::set<QString> &visible);

        /*! Get a relations map without hidden relation attributes. */
#ifdef __clang__
        template<typename T>
#else
        template<RelationsContainerConcept<AllRelations...> T>
#endif
        static RelationsContainer<AllRelations...>
        removeSerializableHiddenRelations(T &&relations, const std::set<QString> &hidden);

        /* Static cast this to a child's instance type (CRTP) */
        TINY_CRTP_MODEL_WITH_BASE_DECLARATIONS
    };

    /* public */

    /* HasRelationships related */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related, template<typename> typename Container>
    const Container<Related *> // NOLINT(readability-const-return-type)
    HasRelationships<Derived, AllRelations...>::getRelationValue(const QString &relation)
    {
        /*! If the key already exists in the relationships hash, it just means the
            relationship has already been loaded, so we'll just return it out of
            here because there is no need to query within the relations twice. */
        if (this->relationLoaded(relation))
            return getRelationFromHash<Related, Container>(relation);

        /*! If the relation is defined on the model, then lazy load and return results
            from the query and hydrate the relationship's value on the "relationships"
            data member m_relations. */
        if (basemodel().getUserRelations().contains(relation))
            return getRelationshipFromMethod<Related, Container>(relation);

        return {};
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related, typename Tag> requires std::same_as<Tag, One>
    Related *
    HasRelationships<Derived, AllRelations...>::getRelationValue(const QString &relation)
    {
        /*! If the key already exists in the relationships hash, it just means the
            relationship has already been loaded, so we'll just return it out of
            here because there is no need to query within the relations twice. */
        if (this->relationLoaded(relation))
            return getRelationFromHash<Related, Tag>(relation);

        /*! If the relation is defined on the model, then lazy load and return results
            from the query and hydrate the relationship's value on the "relationships"
            data member m_relations. */
        if (basemodel().getUserRelations().contains(relation))
            return getRelationshipFromMethod<Related, Tag>(relation);

        return nullptr;
    }

    // TODO make getRelation() Container argument compatible with STL containers API silverqx
    // TODO solve different behavior like Eloquent getRelation() silverqx
    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related, template<typename> typename Container>
    const Container<Related *> // NOLINT(readability-const-return-type)
    HasRelationships<Derived, AllRelations...>::getRelation(const QString &relation)
    {
        if (!relationLoaded(relation))
            throw Exceptions::RelationNotLoadedError(
                    TypeUtils::classPureBasename<Derived>(), relation);

        return getRelationFromHash<Related, Container>(relation);
    }

    /* Returning shared_ptr for getRelation()-related methods would be unperformant,
       imagine 100 models with 2 relations and every relation would have another 100
       models, it's houndreds of shared_ptr-s which would have to be counted. */
    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related, typename Tag> requires std::same_as<Tag, One>
    Related *
    HasRelationships<Derived, AllRelations...>::getRelation(const QString &relation)
    {
        if (!relationLoaded(relation))
            throw Exceptions::RelationNotLoadedError(
                    TypeUtils::classPureBasename<Derived>(), relation);

        return getRelationFromHash<Related, Tag>(relation);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool
    HasRelationships<Derived, AllRelations...>::relationLoaded(
            const QString &relation) const
    {
        return m_relations.contains(relation);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    Derived &
    HasRelationships<Derived, AllRelations...>::setRelation(
            const QString &relation, const ModelsCollection<Related> &models)
    {
        m_relations[relation] = models;

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    Derived &
    HasRelationships<Derived, AllRelations...>::setRelation(
            const QString &relation, ModelsCollection<Related> &&models)
    {
        m_relations[relation] = std::move(models);

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    Derived &
    HasRelationships<Derived, AllRelations...>::setRelation(
            const QString &relation, const std::optional<Related> &model)
    {
        m_relations[relation] = model;

        return this->model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    Derived &
    HasRelationships<Derived, AllRelations...>::setRelation(
            const QString &relation, std::optional<Related> &&model)
    {
        m_relations[relation] = std::move(model);

        return this->model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    requires std::is_base_of_v<Relations::IsPivotModel, Related>
    Derived &
    HasRelationships<Derived, AllRelations...>::setRelation(
            const QString &relation, const std::optional<Related> &model)
    {
        m_pivots.insert(relation);

        m_relations[relation] = model;

        return this->model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    requires std::is_base_of_v<Relations::IsPivotModel, Related>
    Derived &
    HasRelationships<Derived, AllRelations...>::setRelation(
            const QString &relation, std::optional<Related> &&model)
    {
        m_pivots.insert(relation);

        m_relations[relation] = std::move(model);

        return this->model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const RelationsContainer<AllRelations...> &
    HasRelationships<Derived, AllRelations...>::getRelations() const noexcept
    {
        return m_relations;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    RelationsContainer<AllRelations...> &
    HasRelationships<Derived, AllRelations...>::getRelations() noexcept
    {
        return m_relations;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &HasRelationships<Derived, AllRelations...>::unsetRelations()
    {
        m_relations.clear();

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    HasRelationships<Derived, AllRelations...>::unsetRelation(const QString &relation)
    {
        m_relations.erase(relation);

        return model();
    }

    /* Relationships factory methods */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    std::unique_ptr<Relations::HasOne<Derived, Related>>
    HasRelationships<Derived, AllRelations...>::hasOne(
            QString foreignKey, QString localKey)
    {
        auto &model = this->model();

        auto instance = newRelatedInstance<Related>();

        if (foreignKey.isEmpty())
            foreignKey = model.getForeignKey(); // model() needed as it's overridden in the BasePivot

        if (localKey.isEmpty())
            localKey = basemodel().getKeyName();

        return newHasOne<Related>(std::move(instance), model,
                                  DOT_IN.arg(instance->getTable(), foreignKey),
                                  localKey);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    std::unique_ptr<Relations::BelongsTo<Derived, Related>>
    HasRelationships<Derived, AllRelations...>::belongsTo(
            QString foreignKey, QString ownerKey, QString relation)
    {
        /* If no relation name was given, we will use the Related class type to extract
           the name and use that as the relationship name as most of the time this
           will be what we desire to use for the belongsTo relationships. */
        if (relation.isEmpty())
            relation = guessBelongsToRelation<Related>();

        auto instance = newRelatedInstance<Related>();

        const auto &relatedKeyName = instance->getKeyName();

        /* If no foreign key was supplied, we can guess the proper foreign key name
           by using the snake_case name of the relationship, which when combined
           with an "_id" should conventionally match the columns. */
        if (foreignKey.isEmpty())
            foreignKey = QStringLiteral("%1_%2").arg(relation, relatedKeyName);

        /* Once we have the foreign key names, we return the relationship instance,
           which will actually be responsible for retrieving and hydrating every
           relations. */
        if (ownerKey.isEmpty())
            ownerKey = relatedKeyName;

        return newBelongsTo<Related>(std::move(instance), model(),
                                     foreignKey, ownerKey, relation);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    std::unique_ptr<Relations::HasMany<Derived, Related>>
    HasRelationships<Derived, AllRelations...>::hasMany(
            QString foreignKey, QString localKey)
    {
        auto &model = this->model();

        auto instance = newRelatedInstance<Related>();

        if (foreignKey.isEmpty())
            foreignKey = model.getForeignKey(); // model() needed as it's overridden in the BasePivot

        if (localKey.isEmpty())
            localKey = basemodel().getKeyName();

        return newHasMany<Related>(std::move(instance), model,
                                   DOT_IN.arg(instance->getTable(), foreignKey),
                                   localKey);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related, typename PivotType>
    std::unique_ptr<Relations::BelongsToMany<Derived, Related, PivotType>>
    HasRelationships<Derived, AllRelations...>::belongsToMany(
            QString table, QString foreignPivotKey, QString relatedPivotKey,
            QString parentKey, QString relatedKey, QString relation)
    {
        /* If no relation name was given, we will use the Related class type to extract
           the name, suffix it with 's' and use that as the relationship name, as most
           of the time this will be what we desire to use for the belongsToMany
           relationships. */
        if (relation.isEmpty())
            relation = guessBelongsToManyRelation<Related>();

        auto &model = this->model();

        /* First, we'll need to determine the foreign key and "other key"
           for the relationship. Once we have determined the keys, we'll make
           the relationship instance we need for this. */
        auto instance = newRelatedInstance<Related>();

        if (foreignPivotKey.isEmpty())
            foreignPivotKey = model.getForeignKey(); // model() needed as it's overridden in the BasePivot

        if (relatedPivotKey.isEmpty())
            relatedPivotKey = instance->getForeignKey();

        /* If no table name was provided, we can guess it by concatenating the two
           models using underscores in alphabetical order. The two model names
           are transformed to snake_case from their default StudlyCase also. */
        if (table.isEmpty())
            table = pivotTableName<Related>();

        if (parentKey.isEmpty())
            parentKey = basemodel().getKeyName();

        if (relatedKey.isEmpty())
            relatedKey = instance->getKeyName();

        return newBelongsToMany<Related, PivotType>(
                    std::move(instance), model, table, foreignPivotKey,
                    relatedPivotKey, parentKey, relatedKey, relation);
    }

    /* Touching timestamps */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void HasRelationships<Derived, AllRelations...>::touchOwners() const
    {
        for (const auto &relation : getTouchedRelations())
            touchOwnersWithVisitor(relation);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const QStringList &
    HasRelationships<Derived, AllRelations...>::getTouchedRelations() const
    {
        return basemodel().getUserTouches();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    HasRelationships<Derived, AllRelations...>::addTouch(const QString &touch)
    {
        basemodel().getUserTouches().emplaceBack(touch);

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    HasRelationships<Derived, AllRelations...>::addTouch(QString &&touch)
    {
        basemodel().getUserTouches().emplaceBack(std::move(touch));

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    HasRelationships<Derived, AllRelations...>::addTouches(const QStringList &touches)
    {
        basemodel().getUserTouches().append(touches);

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    HasRelationships<Derived, AllRelations...>::addTouches(QStringList &&touches)
    {
        basemodel().getUserTouches().append(std::move(touches));

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    HasRelationships<Derived, AllRelations...>::setTouchedRelations(
            const QStringList &touches)
    {
        basemodel().getUserTouches() = touches;

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    HasRelationships<Derived, AllRelations...>::setTouchedRelations(
            QStringList &&touches)
    {
        basemodel().getUserTouches() = std::move(touches);

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool
    HasRelationships<Derived, AllRelations...>::touches(const QString &relation) const
    {
        return getTouchedRelations().contains(relation);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    HasRelationships<Derived, AllRelations...>::clearTouches()
    {
        basemodel().getUserTouches().clear();

        return model();
    }

    /* Serialization - Relations */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<SerializedAttributes C, typename PivotType>
    C HasRelationships<Derived, AllRelations...>::serializeRelations() const
    {
        const auto serializableRelations = getSerializableRelations();

        C attributes;
        if constexpr (HasReserveMethod<C>)
            attributes.reserve(static_cast<decltype (attributes)::size_type>(
                                   serializableRelations.size()));

        for (const auto &[relation, models] : serializableRelations) {
            Q_ASSERT(!models.valueless_by_exception());

            // Serialize belongs-to-many relation or the pivot model
            if constexpr (hasPivotRelation() && !std::is_void_v<PivotType>) {
                // Pivot model, skipping the relation store, call the visited directly
                if (m_pivots.contains(relation))
                    serializeRelationVisited<PivotType, C, void>(relation, models,
                                                                 attributes);
                // belongs-to-many relation
                else
                    serializeRelationWithVisitor(relation, models, attributes);
            }
            // Serialize has-one, has-many, and belongs-to relations
            else
                serializeRelationWithVisitor(relation, models, attributes);
        }

        return attributes;
    }

    /* Others */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool HasRelationships<Derived, AllRelations...>::operator==( // NOLINT(misc-no-recursion)
            const HasRelationships &right) const
    {
        // u_relations == right.u_relations
        /* It compares only the size and keys and doesn't compare hash values because
           the std::function doesn't have a full/complete operator==() (it only compares
           for the nullptr). */
        if (!compareURelations(u_relations, right.u_relations))
            return false;

        return m_relations == right.m_relations &&
               u_touches   == right.u_touches   &&
               m_pivots    == right.m_pivots;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    constexpr bool
    HasRelationships<Derived, AllRelations...>::hasPivotRelation() noexcept
    {
        return std::disjunction_v<
                std::is_base_of<Relations::IsPivotModel, AllRelations>...>;
    }

    /* protected */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<class Related, template<typename> typename Container>
    Container<Related *>
    HasRelationships<Derived, AllRelations...>::getRelationshipFromMethod(
            const QString &relation)
    {
        // Obtain related models
        auto relatedModels =
                getRelationshipFromMethodWithVisitor<Related,
                                                     ModelsCollection<Related>>(relation);

        setRelation(relation, std::move(relatedModels));

        return getRelationFromHash<Related, Container>(relation);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<class Related, typename Tag> requires std::same_as<Tag, One>
    Related *
    HasRelationships<Derived, AllRelations...>::getRelationshipFromMethod(
            const QString &relation)
    {
        // Obtain related model
        auto relatedModel =
                getRelationshipFromMethodWithVisitor<Related,
                                                     std::optional<Related>>(relation);

        setRelation(relation, std::move(relatedModel));

        return getRelationFromHash<Related, Tag>(relation);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    HasRelationships<Derived, AllRelations...>::setRelations(
            const RelationsContainer<AllRelations...> &relations)
    {
        m_relations = relations;

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    HasRelationships<Derived, AllRelations...>::setRelations(
            RelationsContainer<AllRelations...> &&relations)
    {
        m_relations = std::move(relations);

        return model();
    }

    /* Relationships factory methods */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    std::unique_ptr<Relations::HasOne<Derived, Related>>
    HasRelationships<Derived, AllRelations...>::newHasOne(
            std::unique_ptr<Related> &&related, Derived &parent,
            const QString &foreignKey, const QString &localKey) const
    {
        return Relations::HasOne<Derived, Related>::instance(
                    std::move(related), parent, foreignKey, localKey);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    std::unique_ptr<Relations::BelongsTo<Derived, Related>>
    HasRelationships<Derived, AllRelations...>::newBelongsTo(
            std::unique_ptr<Related> &&related, Derived &child,
            const QString &foreignKey, const QString &ownerKey,
            const QString &relation) const
    {
        return Relations::BelongsTo<Derived, Related>::instance(
                    std::move(related), child, foreignKey, ownerKey, relation);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    std::unique_ptr<Relations::HasMany<Derived, Related>>
    HasRelationships<Derived, AllRelations...>::newHasMany(
            std::unique_ptr<Related> &&related, Derived &parent,
            const QString &foreignKey, const QString &localKey) const
    {
        return Relations::HasMany<Derived, Related>::instance(
                    std::move(related), parent, foreignKey, localKey);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related, typename PivotType>
    std::unique_ptr<Relations::BelongsToMany<Derived, Related, PivotType>>
    HasRelationships<Derived, AllRelations...>::newBelongsToMany(
            std::unique_ptr<Related> &&related, Derived &parent, const QString &table,
            const QString &foreignPivotKey, const QString &relatedPivotKey,
            const QString &parentKey, const QString &relatedKey,
            const QString &relation) const
    {
        return Relations::BelongsToMany<Derived, Related, PivotType>::instance(
                    std::move(related), parent, table, foreignPivotKey,
                    relatedPivotKey, parentKey, relatedKey, relation);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    std::unique_ptr<Related>
    HasRelationships<Derived, AllRelations...>::newRelatedInstance() const
    {
        auto instance = Related::instanceHeap();

        if (instance->getConnectionName().isEmpty())
            instance->setConnection(basemodel().getConnectionName());

        return instance;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    QString
    HasRelationships<Derived, AllRelations...>::guessBelongsToRelation() const
    {
        static const auto relation = guessBelongsToRelationInternal<Related>();

        /* validateUserRelation() method call can not be cached, has to be called
           every time, to correctly inform the user about invalid relation name. */

        // Validate if the guessed relation name exists in the u_relations
        validateUserRelation(relation, RelationFrom::BELONGS_TO);

        return relation;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    QString
    HasRelationships<Derived, AllRelations...>::guessBelongsToManyRelation() const
    {
        static const auto relation = TMPL_PLURAL
                                     .arg(guessBelongsToRelationInternal<Related>());

        /* validateUserRelation() method call can not be cached, has to be called
           every time, to correctly inform the user about invalid relation name. */

        // Validate if the guessed relation name exists in the u_relations
        validateUserRelation(relation, RelationFrom::BELONGS_TO_MANY);

        return relation;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    QString HasRelationships<Derived, AllRelations...>::pivotTableName() const
    {
        /* The joining table name, by convention, is simply the snake_cased, models
           sorted alphabetically and concatenated with an underscore, so we can
           just sort the models and join them together to get the table name. */
        QStringList segments {
            // The table name of the current model instance
            TypeUtils::classPureBasename<Derived>(),
            // The table name of the related model instance
            TypeUtils::classPureBasename<Related>(),
        };

        /* Now that we have the model names in the vector, we can just sort them and
           use the join function to join them together with an underscore,
           which is typically used by convention within the database system. */
        segments.sort(Qt::CaseInsensitive);

        return segments.join(UNDERSCORE).toLower();
    }

    /* Serialization - Relations */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    RelationsContainer<AllRelations...>
    HasRelationships<Derived, AllRelations...>::getSerializableRelations() const
    {
        const auto &relations = getRelations();

        // Nothing to do
        if (relations.empty())
            return {};

        const auto &basemodel = this->basemodel();
        const auto &visible   = basemodel.getUserVisible();
        const auto &hidden    = basemodel.getUserHidden();

        const auto isVisibleEmpty = visible.empty();

        // Nothing to do, the visible and hidden attributes are not defined
        if (isVisibleEmpty && hidden.empty())
            return relations;

        // No need to compute the visible relations (also allows forwarding reference)
        if (isVisibleEmpty)
            return removeSerializableHiddenRelations(relations, hidden);

        // Pass the visible and hidden down to avoid obtaining these references twice
        return removeSerializableHiddenRelations(
                    getSerializableVisibleRelations(relations, visible),
                    hidden);
    }

    /* Others */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool HasRelationships<Derived, AllRelations...>::compareURelations(
            const QHash<QString, RelationVisitor> &left,
            const QHash<QString, RelationVisitor> &right)
    {
        if (left.size() != right.size())
            return false;

        for (auto it = right.keyBegin(); it != right.keyEnd(); ++it)
            if (left.find(*it) == left.constEnd())
                return false;

        return true;
    }

    /* private */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void
    HasRelationships<Derived, AllRelations...>::validateUserRelation(
            const QString &name, const RelationFrom from) const
    {
        // Nothing to do, relation defined
        if (basemodel().getUserRelations().contains(name))
            return;

        throw Exceptions::RelationMappingNotFoundError(
                    TypeUtils::classPureBasename<Derived>(), name, from);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<class Related, template<typename> typename Container>
    Container<Related *>
    HasRelationships<Derived, AllRelations...>::getRelationFromHash(
            const QString &relation)
    {
        auto &relationVariant = m_relations.find(relation)->second;

        // Check relation type to avoid std::bad_variant_access
        checkRelationType<ModelsCollection<Related>, Related>(
                    relationVariant, relation, QStringLiteral("getRelation"));

        /* Obtain related models from data member hash as the ModelsCollection,
           it is internal format and transform it into a Container of pointers
           to related models, so a user can directly modify these models and push or
           save them afterward. */
        namespace views = ranges::views;
        return std::get<ModelsCollection<Related>>(relationVariant)
                | views::transform([](Related &model) -> Related * { return &model; })
                | ranges::to<Container<Related *>>();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<class Related, typename Tag> requires std::same_as<Tag, One>
    Related *
    HasRelationships<Derived, AllRelations...>::getRelationFromHash(
            const QString &relation)
    {
        auto &relationVariant = m_relations.find(relation)->second;

        // Check relation type to avoid std::bad_variant_access
        checkRelationType<std::optional<Related>, Related>(
                    relationVariant, relation, QStringLiteral("getRelation"));

        /* Obtain related model from data member hash and return it as a pointer or
           nullptr if no model is associated, so a user can directly modify this
           model and push or save it afterward. */

        auto &relatedModel = std::get<std::optional<Related>>(relationVariant);

        return relatedModel ? &*relatedModel : nullptr;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related, typename Result> requires (!std::is_reference_v<Result>)
    Result
    HasRelationships<Derived, AllRelations...>::getRelationshipFromMethodWithVisitor(
            const QString &relation) const
    {
        // Throw exception if a relation is not defined
        validateUserRelation(relation);

        // Save model/s to the store to avoid passing variables to the visitor
        this->template createLazyStore<Related>().visit(relation);

        /* Obtain the result, related model/s (can't be a reference as the store is
           destroyed on the next line, so I have to make a copy). */
        const auto lazyResult = this->template lazyStore<Related>().result();

        // Releases the ownership and destroy the top relation store on the stack
        this->resetRelationStore();

        // Check relation type to avoid std::bad_variant_access
        checkRelationType<Result, Related>(lazyResult, relation,
                                           QStringLiteral("getRelationValue"));

#if defined(_MSC_VER) && !defined(__clang__)
#  pragma warning(push)
#  pragma warning(disable : 26816)
#endif
        // We are returning a copy here, MSVC C26816 is the false positive
        return std::get<Result>(lazyResult);
#if defined(_MSC_VER) && !defined(__clang__)
#  pragma warning(pop)
#endif
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Result, typename Related, typename T>
    void HasRelationships<Derived, AllRelations...>::checkRelationType(
            const T &relationVariant, const QString &relation,
            const QString &source) const
    {
        if constexpr (std::is_same_v<Result, std::optional<Related>>) {
            if (!std::holds_alternative<Result>(relationVariant))
                throw Orm::Exceptions::InvalidTemplateArgumentError(
                        QStringLiteral(
                            "The relation '%1' is many type relation, use the "
                            "%2<%3>() method overload without an 'Orm::One' tag.")
                        .arg(relation, source,
                             TypeUtils::classPureBasename<Related>()));

        } else if constexpr (std::is_same_v<Result, ModelsCollection<Related>>) {
            if (!std::holds_alternative<Result>(relationVariant))
                throw Orm::Exceptions::InvalidTemplateArgumentError(
                        QStringLiteral(
                            "The relation '%1' is one type relation, use the "
                            "%2<%3, Orm::One>() method overload with an 'Orm::One' tag.")
                        .arg(relation, source,
                             TypeUtils::classPureBasename<Related>()));
        } else
            throw Orm::Exceptions::InvalidTemplateArgumentError(
                    "Unexpected 'Result' template argument.");
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    QString
    HasRelationships<Derived, AllRelations...>::guessBelongsToRelationInternal() const
    {
        auto relation = TypeUtils::classPureBasename<Related>();

        relation[0] = relation[0].toLower();

        return relation;
    }

    /* Eager load relation store related */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<SameDerivedCollectionModel<Derived> CollectionModel>
    void HasRelationships<Derived, AllRelations...>::eagerLoadRelationWithVisitor(
            const WithItem &relation, const TinyBuilder<Derived> &builder,
            ModelsCollection<CollectionModel> &models) const
    {
        // Throw exception if a relation is not defined
        validateUserRelation(relation.name);

        /* Save the needed variables to the store to avoid passing variables
           to the visitor. */
        this->createEagerStore(builder, models, relation).visit(relation.name);

        // Releases the ownership and destroy the top relation store on the stack
        this->resetRelationStore();
    }

    /* Get related table for belongs-to-many relation store related */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::optional<QString>
    HasRelationships<Derived, AllRelations...>::
    getRelatedTableForBelongsToManyWithVisitor(const QString &relation) const
    {
        // Can't be a nested relation, see a comment in TinyBuilder::parseWithRelations()
        Q_ASSERT(!relation.contains(DOT));

        // Throw exception if a relation is not defined
        validateUserRelation(relation);

        // Create the store and visit relation
        this->createBelongsToManyRelatedTableStore();

        // NRVO kicks in
        auto relatedTable = this->belongsToManyRelatedTableStore()
                            // A little different than other visitors because of caching
                            .visitWithResult(relation);

        // Releases the ownership and destroy the top relation store on the stack
        this->resetRelationStore();

        return relatedTable;
    }

    /* Push relation store related */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool HasRelationships<Derived, AllRelations...>::pushWithVisitor(
            const QString &relation, RelationsType<AllRelations...> &models) const
    {
        // TODO prod remove, I don't exactly know if this can really happen silverqx
        /* Check for empty variant, the std::monostate is at zero index and
           consider it as success to continue 'pushing'. */
        const auto variantIndex = models.index();
        Q_ASSERT(variantIndex > 0);
        if (variantIndex == 0)
            return true;

        // Throw exception if a relation is not defined
        validateUserRelation(relation);

        // Save model/s to the store to avoid passing variables to the visitor
        this->createPushStore(models).visit(relation);

        const auto pushResult = this->pushStore().result();

        // Releases the ownership and destroy the top relation store on the stack
        this->resetRelationStore();

        return pushResult;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    void HasRelationships<Derived, AllRelations...>::pushVisited() const
    {
        const RelationsType<AllRelations...> &models = this->pushStore().models();

        // Invoke pushVisited() on the base of hold alternative in the models
        if (std::holds_alternative<ModelsCollection<Related>>(models))
            pushVisited<Related, Many>();

        else if (std::holds_alternative<std::optional<Related>>(models))
            pushVisited<Related, One>();

        else
            throw Orm::Exceptions::RuntimeError(
                    "this->pushStore().models() holds unexpected alternative.");
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related, typename Tag> requires std::same_as<Tag, Many>
    void HasRelationships<Derived, AllRelations...>::pushVisited() const
    {
        auto &pushStore = this->pushStore();

        for (auto &model : std::get<ModelsCollection<Related>>(pushStore.models()))
            if (!model.push())
                return pushStore.setResult(false); // clazy:exclude=returning-void-expression

        pushStore.setResult(true);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related, typename Tag> requires std::same_as<Tag, One>
    void HasRelationships<Derived, AllRelations...>::pushVisited() const
    {
        auto &pushStore = this->pushStore();

        auto &model = std::get<std::optional<Related>>(pushStore.models());
        Q_ASSERT(model);

        // Skip a null model, consider it as success
        if (!model)
            return pushStore.setResult(true); // clazy:exclude=returning-void-expression

        pushStore.setResult(model->push());
    }

    /* Touch owners store related */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void
    HasRelationships<Derived, AllRelations...>::touchOwnersWithVisitor(
            const QString &relation) const
    {
        // Throw exception if a relation is not defined
        validateUserRelation(relation);

        // Save model/s to the store to avoid passing variables to the visitor
        this->createTouchOwnersStore(relation).visit(relation);

        // Releases the ownership and destroy the top relation store on the stack
        this->resetRelationStore();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related, typename Relation>
    void HasRelationships<Derived, AllRelations...>::touchOwnersVisited(
            const Relation &relation, [[maybe_unused]] const QString &relationName)
    {
        relation->touch();

        // Many type relation
        if constexpr (std::is_base_of_v<Relations::IsManyRelation,
                                        typename Relation::element_type>)
        {
            for (auto *const relatedModel : getRelationValue<Related>(relationName))
                /* I have checked it more times and the getRelation/Value() related
                   methods can't contain the nullptr in any case but I leave this check
                   here anyway. */
                if (relatedModel)
                    relatedModel->touchOwners();
        }

        // One type relation
        else if constexpr (std::is_base_of_v<Relations::IsOneRelation,
                                             typename Relation::element_type>)
        {
            if (auto *const relatedModel = getRelationValue<Related, One>(relationName);
                relatedModel
            )
                relatedModel->touchOwners();
        } else
            throw Orm::Exceptions::InvalidTemplateArgumentError(
                    "Bad relation type passed to the Model::touchOwnersVisited().");
    }

    /* QueriesRelationships store related */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    void HasRelationships<Derived, AllRelations...>::queriesRelationshipsWithVisitor(
            const QString &relation, Concerns::QueriesRelationships<Derived> &origin,
            const QString &comparison, const qint64 count, const QString &condition,
            const std::function<void(
                Concerns::QueriesRelationshipsCallback<Related> &)> &callback,
            const std::optional<std::reference_wrapper<QStringList>> relations) const
    {
        // Throw exception if a relation is not defined
        validateUserRelation(relation);

        // Save model/s to the store to avoid passing variables to the visitor
        this->template createQueriesRelationshipsStore<Related>(
                    origin, comparison, count, condition, callback, relations)
                .visit(relation);

        // Releases the ownership and destroy the top relation store on the stack
        this->resetRelationStore();
    }

    /* Operations on a Model instance */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QList<WithItem>
    HasRelationships<Derived, AllRelations...>::getLoadedRelationsWithoutPivot() const
    {
        QList<WithItem> relations;
        relations.reserve(static_cast<decltype (relations)::size_type>(
                              m_relations.size()));

        /* Get all currently loaded relation names except pivot relations. We need
           to check for the pivot models, but only if the std::variant which holds
           relations also holds a pivot model alternative, otherwise it is useless. */
        for (const auto &relation : m_relations) {
            const auto &relationName = relation.first;

            // Skip pivot relations
            if constexpr (hasPivotRelation())
                if (m_pivots.contains(relationName))
                    continue;

            relations.append({relationName});
        }

        return relations;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void HasRelationships<Derived, AllRelations...>::replaceRelations(
            RelationsContainer<AllRelations...> &relations,
            const QList<WithItem> &onlyRelations)
    {
        /* Replace only relations which was passed to this method, leave other
           relations untouched. */
        for (auto itRelation = relations.begin(); itRelation != relations.end();
             ++itRelation)
        {
            const auto &key = itRelation->first;

            const auto relationsContainKey =
                    ranges::contains(onlyRelations, true, [&key](const auto &relation)
            {
                const auto &relationName = relation.name;

                if (!relationName.contains(COLON))
                    return relationName == key;

                // Support for select constraints
                return QStringView(relationName.constData(),
                                   relationName.indexOf(COLON)).trimmed() == key;
            });

            if (!relationsContainKey)
                continue;

            m_relations[key] = std::move(itRelation->second);
        }
    }

    /* Serialization - Relations */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<SerializedAttributes C>
    void HasRelationships<Derived, AllRelations...>::serializeRelationWithVisitor(
            const QString &relation, const RelationsType<AllRelations...> &models,
            C &attributes) const
    {
        // Save model/s to the store to avoid passing variables to the visitor
        this->createSerializeRelationStore(relation, models, attributes)
                .visit(relation);

        // Releases the ownership and destroy the top relation store on the stack
        this->resetRelationStore();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related, SerializedAttributes C, typename PivotType>
    void HasRelationships<Derived, AllRelations...>::serializeRelationVisited(
            QString relation, const RelationsType<AllRelations...> &models,
            C &attributes) const
    {
        QVariant relationSerialized;

        /*! Determine whether the toMap() or toList() was invoked. */
        constexpr auto IsMap = std::is_same_v<C, QVariantMap>;

        // Many type relationship
        if (std::holds_alternative<ModelsCollection<Related>>(models))
            serializeRelation<Related, IsMap, PivotType>(
                        relationSerialized, std::get<ModelsCollection<Related>>(models));

        // One type relationship
        else if (std::holds_alternative<std::optional<Related>>(models))
            // No need to pass the PivotType down for the one type relation
            serializeRelation<Related, IsMap>(
                        relationSerialized, std::get<std::optional<Related>>(models));

        else
            Q_UNREACHABLE();

        /* Practically useless because the "if" checks above check all possible
           cases, but I leave it here anyway. */
        Q_ASSERT(relationSerialized.isValid());

        /* If the relationships snake-casing is enabled, we will snake_case this
           key so that the relation attribute is snake_cased in this returned
           map to the developers, making this consistent with attributes. */
        if (basemodel().getUserSnakeAttributes())
            relation = StringUtils::snake(std::move(relation));

        /* Insert or emplace the serialized relation attributes to the final attributes
           map or vector. */
        insertSerializedRelation(attributes, std::move(relation),
                                 std::move(relationSerialized));
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related, bool IsMap, typename PivotType>
    void HasRelationships<Derived, AllRelations...>::serializeRelation(
            QVariant &relationSerialized, const ModelsCollection<Related> &models)
    {
        if constexpr (IsMap)
            relationSerialized.setValue(
                models.template toMapVariantList<PivotType>());
        else
            relationSerialized.setValue(
                models.template toListVariantList<PivotType>());
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related, bool IsMap>
    void HasRelationships<Derived, AllRelations...>::serializeRelation(
            QVariant &relationSerialized, const std::optional<Related> &model)
    {
        // No need to pass the PivotType down for the one type relation
        if (model) {
            if constexpr (IsMap)
                relationSerialized.setValue(model->toMap());
            else
                relationSerialized.setValue(model->toList());
        }
        else // A NULL foreign key
            relationSerialized.setValue(nullptr);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void HasRelationships<Derived, AllRelations...>::insertSerializedRelation(
            QVariantMap &attributes, const QString &relation,
            const QVariant &relationSerialized)
    {
        attributes.insert(relation, relationSerialized);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void HasRelationships<Derived, AllRelations...>::insertSerializedRelation(
            QList<AttributeItem> &attributes, QString &&relation,
            QVariant &&relationSerialized)
    {
        attributes.emplaceBack(std::move(relation),
                               std::move(relationSerialized));
    }

    /* Serialization - HidesAttributes */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    RelationsContainer<AllRelations...>
    HasRelationships<Derived, AllRelations...>::getSerializableVisibleRelations(
            const RelationsContainer<AllRelations...> &relations,
            const std::set<QString> &visible)
    {
        // Get visible relations only
        /* Compute visible keys on relations map, the intersection is needed to compute
           only keys that really exists. */
        std::set<QString> visibleKeys;
        ranges::set_intersection(
                    AttributeUtils::keys<AllRelations...>(relations), visible,
                    ranges::inserter(visibleKeys, visibleKeys.cend()));

        RelationsContainerType serializableRelations;
        if constexpr (HasReserveMethod<RelationsContainerType>)
            serializableRelations.reserve(relations.size());

        for (const auto &[key, value] : relations)
            if (visibleKeys.contains(key))
                serializableRelations.emplace(key, value);

        return serializableRelations;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
#ifdef __clang__
    template<typename T>
#else
    template<RelationsContainerConcept<AllRelations...> T>
#endif
    RelationsContainer<AllRelations...>
    HasRelationships<Derived, AllRelations...>::removeSerializableHiddenRelations(
            T &&relations, const std::set<QString> &hidden)
    {
        // Nothing to do
        if (hidden.empty())
            return std::forward<T>(relations);

        /* Remove hidden relations, from the map container returned by
           the getSerializableVisibleRelations()! */
        /* Compute hidden keys on relations map, the intersection is needed to compute
           only keys that really exists. */
        std::set<QString> hiddenKeys;
        ranges::set_intersection(
                    AttributeUtils::keys<AllRelations...>(relations), hidden,
                    ranges::inserter(hiddenKeys, hiddenKeys.cend()));

        RelationsContainerType serializableRelations;
        if constexpr (HasReserveMethod<RelationsContainerType>)
            serializableRelations.reserve(relations.size());

        for (auto &&[key, value] : relations)
            if (!hiddenKeys.contains(key))
                serializableRelations.emplace(std::forward<decltype (key)>(key),
                                              std::forward<decltype (value)>(value)); // try_emplace() not needed

        return serializableRelations;
    }

    /* Static cast this to a child's instance type (CRTP) */

    TINY_CRTP_MODEL_WITH_BASE_DEFINITIONS(HasRelationships)

} // namespace Concerns
} // namespace Orm::Tiny

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_CONCERNS_HASRELATIONSHIPS_HPP
