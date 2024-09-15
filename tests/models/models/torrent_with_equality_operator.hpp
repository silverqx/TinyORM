#pragma once
#ifndef MODELS_TORRENT_WITH_EQUALITY_OPERATOR_HPP
#define MODELS_TORRENT_WITH_EQUALITY_OPERATOR_HPP

#include "orm/tiny/model.hpp"

namespace Models
{

using Orm::Tiny::Model;

class Torrent_With_Equality_Operator final : public Model<Torrent_With_Equality_Operator> // NOLINT(bugprone-exception-escape)
{
    friend Model;
    using Model::Model;

public:
    /*! Equality comparison operator for the Torrent_With_Equality_Operator. */
    bool operator==(const Torrent_With_Equality_Operator &) const = default;

private:
    /*! The table associated with the model. */
    QString u_table {u"torrents"_s};
};

/*! Dummy type that helps to define the overloaded operator==(). */
struct DummyType
{};

class Torrent_With_Equality_Operator_Overload final : // NOLINT(bugprone-exception-escape)
        public Model<Torrent_With_Equality_Operator_Overload>
{
    friend Model;
    using Model::Model;

public:
    /*! Equality comparison operator for the Torrent_With_Equality_Operator. */
    bool operator==(const Torrent_With_Equality_Operator_Overload &) const = default;

    /*! Equality comparison operator overload for the DummyType. */
    bool operator==(const DummyType &/*unused*/) const
    {
        return true;
    }

private:
    /*! The table associated with the model. */
    QString u_table {u"torrents"_s};

    /*! Indicates whether to skip attributes comparison in the Derived model. */
    constexpr static auto u_skipCompareDerived = true;
};

} // namespace Models

#endif // MODELS_TORRENT_WITH_EQUALITY_OPERATOR_HPP
