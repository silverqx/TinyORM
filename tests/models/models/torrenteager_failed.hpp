#pragma once
#ifndef MODELS_TORRENTEAGER_FAILED_HPP
#define MODELS_TORRENTEAGER_FAILED_HPP

#include "orm/tiny/model.hpp"

namespace Models
{

using Orm::Tiny::Model;

class TorrentEager_Failed final : public Model<TorrentEager_Failed> // NOLINT(bugprone-exception-escape)
{
    friend Model;
    using Model::Model;

    /*! The table associated with the model. */
    QString u_table {u"torrents"_s};

    /*! The relations to eager load on every query. */
    QList<QString> u_with {
        u"torrentFiles-NON_EXISTENT"_s,
    };
};

} // namespace Models

#endif // MODELS_TORRENTEAGER_FAILED_HPP
