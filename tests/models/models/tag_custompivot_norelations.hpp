#pragma once
#ifndef MODELS_TAG_CUSTOMPIVOT_NORELATIONS_HPP
#define MODELS_TAG_CUSTOMPIVOT_NORELATIONS_HPP

#include "orm/tiny/model.hpp"

namespace Models
{

using Orm::Tiny::Model;

class Tagged;

class Tag_CustomPivot_NoRelations final : // NOLINT(bugprone-exception-escape)
        public Model<Tag_CustomPivot_NoRelations, Tagged>
{
    friend Model;
    using Model::Model;

    /*! The table associated with the model. */
    QString u_table {u"torrent_tags"_s};
};

} // namespace Models

#endif // MODELS_TAG_CUSTOMPIVOT_NORELATIONS_HPP
