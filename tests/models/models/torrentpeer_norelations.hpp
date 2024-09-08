#pragma once
#ifndef MODELS_TORRENTPEER_NORELATIONS_HPP
#define MODELS_TORRENTPEER_NORELATIONS_HPP

#include "orm/tiny/model.hpp"

namespace Models
{

using Orm::Tiny::Model;

class TorrentPeer_NoRelations final : public Model<TorrentPeer_NoRelations> // NOLINT(bugprone-exception-escape)
{
    friend Model;
    using Model::Model;

    /*! The table associated with the model. */
    QString u_table {u"torrent_peers"_s};
};

} // namespace Models

#endif // MODELS_TORRENTPEER_NORELATIONS_HPP
