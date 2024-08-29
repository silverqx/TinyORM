#pragma once
#ifndef MODELS_TAG_CUSTOMPIVOT_NORELATIONS_HPP
#define MODELS_TAG_CUSTOMPIVOT_NORELATIONS_HPP

#include "orm/tiny/model.hpp"

namespace Models
{

using Orm::Tiny::Model;

class Tagged;

class Tag_CustomPivot_NoRelations final : // NOLINT(bugprone-exception-escape, misc-no-recursion)
        public Model<Tag_CustomPivot_NoRelations, Tagged>
{
    friend Model;
    using Model::Model;

    /*! The table associated with the model. */
    QString u_table {"torrent_tags"};
};

} // namespace Models

#endif // MODELS_TAG_CUSTOMPIVOT_NORELATIONS_HPP