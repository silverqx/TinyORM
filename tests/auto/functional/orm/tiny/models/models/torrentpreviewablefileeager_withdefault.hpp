#ifndef TORRENTPREVIEWABLEFILEEAGER_WITHDEFAULT_H
#define TORRENTPREVIEWABLEFILEEAGER_WITHDEFAULT_H

#include "orm/tiny/model.hpp"

#include "models/torrenteager_withdefault.hpp"
#include "models/torrentpreviewablefilepropertyeager.hpp"

using Orm::Tiny::Model;
using Orm::Tiny::Relations::Relation;
using Orm::WithItem;

class TorrentPreviewableFileEager_WithDefault final :
        public Model<TorrentPreviewableFileEager_WithDefault, TorrentEager_WithDefault,
                     TorrentPreviewableFilePropertyEager>
{
    friend Model;
    using Model::Model;

public:
    /*! Get the torrent that owns the previewable file. */
    std::unique_ptr<Relation<TorrentPreviewableFileEager_WithDefault,
                             TorrentEager_WithDefault>>
    torrent()
    {
        return belongsTo<TorrentEager_WithDefault>();
    }

    /*! Get the torrent that owns the previewable file. */
    std::unique_ptr<Relation<TorrentPreviewableFileEager_WithDefault,
                             TorrentEager_WithDefault>>
    torrent_WithBoolDefault()
    {
        // Ownership of a unique_ptr()
        auto relation = belongsTo<TorrentEager_WithDefault>();

        relation->withDefault();

        return relation;
    }

    /*! Get the torrent that owns the previewable file. */
    std::unique_ptr<Relation<TorrentPreviewableFileEager_WithDefault,
                             TorrentEager_WithDefault>>
    torrent_WithVectorDefaults()
    {
        // Ownership of a unique_ptr()
        auto relation = belongsTo<TorrentEager_WithDefault>();

        relation->withDefault({{"name", "default_torrent_name"}, {"size", 123}});

        return relation;
    }

    /*! Get the file property associated with the previewable file. */
    std::unique_ptr<Relation<TorrentPreviewableFileEager_WithDefault,
                             TorrentPreviewableFilePropertyEager>>
    fileProperty()
    {
        return hasOne<TorrentPreviewableFilePropertyEager>("previewable_file_id");
    }

    /*! Get the file property associated with the previewable file. */
    std::unique_ptr<Relation<TorrentPreviewableFileEager_WithDefault,
                             TorrentPreviewableFilePropertyEager>>
    fileProperty_WithBoolDefault()
    {
        auto relation =
                hasOne<TorrentPreviewableFilePropertyEager>("previewable_file_id");

        relation->withDefault();

        return relation;
    }

    /*! Get the file property associated with the previewable file. */
    std::unique_ptr<Relation<TorrentPreviewableFileEager_WithDefault,
                             TorrentPreviewableFilePropertyEager>>
    fileProperty_WithVectorDefaults()
    {
        auto relation =
                hasOne<TorrentPreviewableFilePropertyEager>("previewable_file_id");

        relation->withDefault({{"name", "default_fileproperty_name"}, {"size", 321}});

        return relation;
    }

private:
    /*! The table associated with the model. */
    QString u_table {"torrent_previewable_files"};

    /*! Map of relation names to methods. */
    QHash<QString, RelationVisitor> u_relations {
        {"torrent",                         [](auto &v) { v(&TorrentPreviewableFileEager_WithDefault::torrent); }},
        {"torrent_WithBoolDefault",         [](auto &v) { v(&TorrentPreviewableFileEager_WithDefault::torrent_WithBoolDefault); }},
        {"torrent_WithVectorDefaults",      [](auto &v) { v(&TorrentPreviewableFileEager_WithDefault::torrent_WithVectorDefaults); }},
        {"fileProperty",                    [](auto &v) { v(&TorrentPreviewableFileEager_WithDefault::fileProperty); }},
        {"fileProperty_WithBoolDefault",    [](auto &v) { v(&TorrentPreviewableFileEager_WithDefault::fileProperty_WithBoolDefault); }},
        {"fileProperty_WithVectorDefaults", [](auto &v) { v(&TorrentPreviewableFileEager_WithDefault::fileProperty_WithVectorDefaults); }},
    };

    /*! The relations to eager load on every query. */
    QVector<WithItem> u_with {
        {"torrent"},
        {"torrent_WithBoolDefault"},
        {"torrent_WithVectorDefaults"},
        {"fileProperty"},
        {"fileProperty_WithBoolDefault"},
        {"fileProperty_WithVectorDefaults"},
    };

    /*! The attributes that are mass assignable. */
    inline static QStringList u_fillable {
        "file_index",
        "filepath",
        "size",
        "progress",
        "note",
    };
};

#endif // TORRENTPREVIEWABLEFILEEAGER_WITHDEFAULT_H
