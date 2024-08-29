#pragma once
#ifndef MODELS_TORRENT_NORELATIONS_HPP
#define MODELS_TORRENT_NORELATIONS_HPP

#include "orm/tiny/model.hpp"

namespace Models
{

using Orm::Tiny::Model;

class Torrent_NoRelations final : public Model<Torrent_NoRelations> // NOLINT(bugprone-exception-escape)
{
    friend Model;
    using Model::Model;

    /*! The table associated with the model. */
    QString u_table {"torrents"};
};

} // namespace Models

#endif // MODELS_TORRENT_NORELATIONS_HPP
