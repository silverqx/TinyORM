#ifndef TORRENTPEEREAGER_NORELATIONS_H
#define TORRENTPEEREAGER_NORELATIONS_H

#include "orm/tiny/basemodel.hpp"

class TorrentPeerEager_NoRelations final :
        public Orm::Tiny::BaseModel<TorrentPeerEager_NoRelations>
{
public:
    friend class BaseModel;

    using BaseModel::BaseModel;

private:
    /*! The table associated with the model. */
    QString u_table {"torrent_peers"};

#ifdef PROJECT_TINYORM_TEST
    /*! The connection name for the model. */
    QString u_connection {"tinyorm_mysql_tests"};
#endif
};

#endif // TORRENTPEEREAGER_NORELATIONS_H
