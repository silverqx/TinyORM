#pragma once
#ifndef MODELS_TAG_RETURNRELATION_HPP
#define MODELS_TAG_RETURNRELATION_HPP

#include "orm/tiny/relations/pivot.hpp"

#include "models/torrent_returnrelation.hpp"

namespace Models
{

using Orm::Tiny::Model;
using Orm::Tiny::Relations::Pivot;

class Torrent_ReturnRelation;

// NOLINTNEXTLINE(misc-no-recursion, bugprone-exception-escape)
class Tag_ReturnRelation final : public Model<Tag_ReturnRelation, Torrent_ReturnRelation,
                                              Pivot>
{
    friend Model;
    using Model::Model;

    /*! The table associated with the model. */
    QString u_table {"torrent_tags"};
};

} // namespace Models

#endif // MODELS_TAG_RETURNRELATION_HPP
