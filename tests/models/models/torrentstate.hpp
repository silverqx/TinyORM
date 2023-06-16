#pragma once
#ifndef MODELS_TORRENTSTATE_HPP
#define MODELS_TORRENTSTATE_HPP

#include "orm/tiny/relations/pivot.hpp"

#include "models/torrent.hpp"

namespace Models
{

using Orm::Constants::NAME;
using Orm::Tiny::Model;
using Orm::Tiny::Relations::Pivot;

class Torrent;

// NOLINTNEXTLINE(misc-no-recursion, bugprone-exception-escape)
class TorrentState final : public Model<TorrentState, Pivot>
{
    friend Model;
    using Model::Model;

private:
    /*! The attributes that are mass assignable. */
    inline static const QStringList u_fillable { // NOLINT(cppcoreguidelines-interfaces-global-init)
        NAME,
    };

    /*! Indicates whether the model should be timestamped. */
    bool u_timestamps = false;
};

} // namespace Models

#endif // MODELS_TORRENTSTATE_HPP
