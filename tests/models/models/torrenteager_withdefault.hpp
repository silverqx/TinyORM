#pragma once
#ifndef TORRENTEAGER_WITHDEFAULT_HPP
#define TORRENTEAGER_WITHDEFAULT_HPP

#include "orm/tiny/model.hpp"

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

#endif // TORRENTEAGER_WITHDEFAULT_HPP
