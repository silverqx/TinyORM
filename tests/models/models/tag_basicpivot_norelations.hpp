#pragma once
#ifndef MODELS_TAG_BASICPIVOT_NORELATIONS_HPP
#define MODELS_TAG_BASICPIVOT_NORELATIONS_HPP

#include "orm/tiny/model.hpp"

namespace Models
{

using Orm::Tiny::Model;
using Orm::Tiny::Relations::Pivot;

class Tag_BasicPivot_NoRelations final : public Model<Tag_BasicPivot_NoRelations, Pivot> // NOLINT(bugprone-exception-escape, misc-no-recursion)
{
    friend Model;
    using Model::Model;

    /*! The table associated with the model. */
    QString u_table {"torrent_tags"};
};

} // namespace Models

#endif // MODELS_TAG_BASICPIVOT_NORELATIONS_HPP
