#pragma once
#ifndef MODELS_TORRENTEAGER_WITHDEFAULT_HPP
#define MODELS_TORRENTEAGER_WITHDEFAULT_HPP

#include "orm/tiny/model.hpp"

namespace Models
{

using Orm::Tiny::Model;

class TorrentEager_WithDefault final : public Model<TorrentEager_WithDefault>
{
    friend Model;
    using Model::Model;

    /*! The table associated with the model. */
    QString u_table {"torrents"};

    /*! The attributes that should be mutated to dates. @deprecated */
    inline static QStringList u_dates {"added_on"};
};

} // namespace Models

#endif // MODELS_TORRENTEAGER_WITHDEFAULT_HPP
