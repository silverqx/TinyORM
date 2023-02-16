#pragma once
#ifndef MODELS_TORRENTPEEREAGER_NORELATIONS_HPP
#define MODELS_TORRENTPEEREAGER_NORELATIONS_HPP

#include "orm/tiny/model.hpp"

namespace Models
{

using Orm::Tiny::Model;

// NOLINTNEXTLINE(bugprone-exception-escape)
class TorrentPeerEager_NoRelations final : public Model<TorrentPeerEager_NoRelations>
{
    friend Model;
    using Model::Model;

    /*! The table associated with the model. */
    QString u_table {"torrent_peers"};
};

} // namespace Models

#endif // MODELS_TORRENTPEEREAGER_NORELATIONS_HPP
