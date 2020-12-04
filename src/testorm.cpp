#include "testorm.h"

#include <QDebug>

#include <iostream>

#include "torrent.h"
#include "torrentpreviewablefile.h"
#include "torrentpeer.h"
#include <orm/ormtypes.h>
#include <range/v3/all.hpp>

using namespace ranges;

// TODO also investigate &&, when to use, I also see usage in for range loops for values silverqx

void TestOrm::run()
{
//    anotherTests();
    testTinyOrm();
//    testQueryBuilder();
}

void TestOrm::anotherTests()
{
//    printf("Function name: %s\n", __FUNCTION__);
//    printf("Decorated function name: %s\n", __FUNCDNAME__);
//    printf("Function signature: %s\n", __FUNCSIG__);

    qt_noop();
}

void TestOrm::testTinyOrm()
{
    TorrentPreviewableFile a;
    auto files = a.query()->where("torrent_id", "=", 2).get();
////    auto files = a.query()->where("torrent_id", "=", 261).get();
////    auto torrent1 = files.first().getRelation<Torrent, QVector>("torrent");
    auto torrent1 = files.first().getRelation<Torrent, Orm::One>("torrent");
//    qDebug() << torrent1->getAttribute("name");
    auto peer1 = torrent1->getRelation<TorrentPeer, Orm::One>("torrentPeer");
////    auto torrent2 = files.first().getRelationValue<Torrent>("torrent");
//    auto torrent3 = files.first().getRelationValue<Torrent, Orm::One>("torrent");
//    auto torrent3 = a.getRelationValue<Torrent, Orm::One>("torrent");
////    qDebug() << torrent2;
////    qDebug() << torrent3.has_value();
    qt_noop();

//    qMetaTypeId<KeyType>();
//    QVector<quint64> vec {12, 6, 8, 2, 7, 8, 8};
//    QVector<QVariant> var(vec.begin(), vec.end());

//    std::sort(vec.begin(), vec.end());
//    std::sort(var.begin(), var.end(), [](QVariant a, QVariant b) {
//        return a.value<KeyType>() < b.value<KeyType>();
//    });
////    std::unique(var.begin(), var.end());
//    auto last = std::unique(vec.begin(), vec.end());
//    vec.erase(last, vec.end());
//    qDebug() << vec;
//    auto last1 = std::unique(var.begin(), var.end(), [](QVariant a, QVariant b) {
//        return a.value<KeyType>() == b.value<KeyType>();
//    });
//    var.erase(last1, var.end());
//    qDebug() << var;

//    using namespace ranges;
//    vec |= actions::sort | actions::unique;
//    qDebug() << vec;
//    using namespace ranges;
////    var |= actions::sort([](QVariant a, QVariant b) {
////           return a.value<KeyType>() == b.value<KeyType>();
////       })/* | actions::unique*/;
//    auto nn = std::move(var) | actions::sort(less {}, &QVariant::value<KeyType>) | actions::unique;
////    var |= actions::sort(less {}, &QVariant::value<KeyType>) | actions::unique;
//    qDebug() << nn;
//    qt_noop();

//    Torrent b;
//    std::any x = b;
//    qDebug() << x.type().name();
//    std::any a = 1;
//    qDebug() << a.type().name() << ": " << std::any_cast<int>(a) << '\n';
//    auto aa = std::any_cast<Torrent>(x);

//    std::variant<int, QString> v {10};
//    auto &x = std::get<int>(v);
//    x = 11;
//    auto y = std::get<int>(v);
//    qDebug() << x;
//    qDebug() << y;

//    QVector<QVariantMap> values;
//    auto nums = views::iota(1, 1000);
//    for (const auto &i : nums)
//        values.append({{"torrent_id", 7}, {"file_index", 0},
//                       {"filepath", QStringLiteral("test7_file%1.mkv").arg(i)},
//                       {"size", i}, {"progress", 50}});

//    m_em.queryBuilder()->from("torrent_previewable_files")
//            .insert(values);
//    qt_noop();

    Torrent b;
    auto torrents = b.query()->where("id", "=", 2).get();
    auto peer = torrents.first().getRelation<TorrentPeer, Orm::One>("torrentPeer");
//////    auto peer = torrents.first().getRelationValue<TorrentPeer, Orm::One>("torrentPeer");
//////    auto peer = b.getRelationValue<TorrentPeer, Orm::One>("torrentPeer");
////    qDebug() << "peers :" << !!peer;
//    auto files = torrents.first().getRelation<TorrentPreviewableFile>("torrentFiles");
//////    auto files = b.getRelationValue<TorrentPreviewableFile>("torrentFiles");
    qt_noop();

    /* Model::with() */
//    {
//        auto torrents = Torrent().with("torrentPeer")->get();
//        auto peer = torrents[1].getRelation<TorrentPeer, Orm::One>("torrentPeer");
//        qDebug() << peer->getAttribute("id");
//        qt_noop();
//    }

//    {
//        auto torrents = Torrent().with("torrentFiles")->get();
//        auto files = torrents[0].getRelation<TorrentPreviewableFile>("torrentFiles");
//        for (const auto &file : files)
//            qDebug() << file.getAttribute("filepath");
//        qt_noop();
//    }

    qt_noop();
}

void TestOrm::testQueryBuilder()
{
//    /* RANGES range-v3 lib */
//    auto v = std::vector<std::string> {"apple", "banana", "kiwi"};
//    for (auto&& [first, second] : v | ranges::views::enumerate)
//         qDebug() << first << ", " << QString::fromStdString(second);
//    qt_noop();

////    std::vector<int> numbers = { 1, 2, 3, 4, 5, 6 };
//    QVector<int> numbers { 1, 2, 3, 4, 5, 6 };

//    auto results = numbers
//                   | ranges::views::filter([](int n){ return n % 2 == 0; })
//                   | ranges::views::transform([](int n){ return n * 2; });

//    for (const auto &v : results)
//        qDebug() << v;
//    qt_noop();

//    /* QT PROPERTY system */
//    MyClass *myinstance = new MyClass;
//    auto r1 = myinstance->setProperty("name", 10);
//    auto v1 = myinstance->property("name");
//    auto dp = myinstance->dynamicPropertyNames();
//    for (auto &prop : dp)
//        qDebug() << prop;
//    qt_noop();

    /* distinct, limit, offset and ordering  */
//    auto a = m_em.queryBuilder()->from("torrents")
////             .distinct()
////             .where("progress", ">", 100)
////             .limit(5)
////             .offset(2)
////             .forPage(2, 5)
//             .orderBy("name")
//             .reorder()
//             .orderBy("id", "desc")
////             .orderByDesc("size")
//             .get();
//    qDebug() << "FIRST :" << a.executedQuery();
//    while (a.next()) {
//        qDebug() << "id :" << a.value("id") << "; name :" << a.value("name");
//    }
//    qt_noop();

//    auto [ok, b] = m_em.queryBuilder()->from("torrents").get({"id, name"});
//    qDebug() << "SECOND :" << b.executedQuery();
//    while (b.next()) {
//        qDebug() << "id :" << b.value("id") << "; name :" << b.value("name");
//    }
//    qt_noop();

    /* WHERE */
//    auto [ok, c] = m_em.queryBuilder()->from("torrents")
//             .where("name", "=", "Internat - S1", "and")
//             .get({"id", "name"});
//    qDebug() << "THIRD :" << c.executedQuery();
//    while (c.next()) {
//        qDebug() << "id :" << c.value("id") << "; name :" << c.value("name");
//    }
//    qt_noop();

    /* also nested where */
//    auto [ok, d] = m_em.queryBuilder()->from("torrents")
//             .where("name", "=", "aliens", "and")
////             .where("id", "=", 1, "and")
////             .where("id", "=", 262, "and")
////             .where("id", "=", 261, "or")
//             .where([](auto &query)
//    {
////        query.where("id", "=", 261, "or");
//        query.where("id", "=", 258, "or");
////        query.where("name", "=", "Most II", "or");
//        query.where([](auto &query)
//        {
//            query.where("id", "=", 5);
//            query.where("name", "=", "Transformers 2");
//        }, "or");
//    }, "or")
//             .get({"id, name"});
//    qDebug() << "FOURTH :" << d.executedQuery();
//    while (d.next()) {
//        qDebug() << "id :" << d.value("id") << "; name :" << d.value("name");
//    }
//    qt_noop();

    // whereIn
//    auto [ok_c1, c1] = m_em.queryBuilder()->from("torrents")
//             .where("name", "=", "Internat - S1", "and")
//             .get({"id", "name"});
//    qDebug() << "whereIn :" << c1.executedQuery();
//    while (c1.next()) {
//        qDebug() << "id :" << c1.value("id") << "; name :" << c1.value("name");
//    }
//    qt_noop();

    /* JOINs */
//    auto e = m_em.queryBuilder()->from("torrents")
//             .rightJoin("torrent_previewable_files", "torrents.id", "=", "torrent_id")
//             .where("torrents.id", "=", 256)
//             .get({"torrents.id", "name", "file_index", "filepath"});
//    qDebug() << "FIFTH :" << e.executedQuery();
//    while (e.next()) {
//        qDebug() << "id :" << e.value("id") << "; name :" << e.value("name")
//                 << "file_index :" << e.value("file_index")
//                 << "filepath :" << e.value("filepath");
//    }

//    auto e = m_em.queryBuilder()->from("torrents")
//             .join("torrent_previewable_files", [](auto &join)
//    {
//        join.on("torrents.id", "=", "torrent_id")
//                .where("torrents.progress", "<", 20);
//    })
////             .where("torrents.id", "=", 256)
//             .get({"torrents.id", "name", "file_index", "filepath"});
//    qDebug() << "SIXTH :" << e.executedQuery();
//    while (e.next()) {
//        qDebug() << "id :" << e.value("id") << "; name :" << e.value("name")
//                 << "file_index :" << e.value("file_index")
//                 << "filepath :" << e.value("filepath");
//    }
//    qt_noop();

    /* first and find */
//    auto f = m_em.queryBuilder()->from("torrents")
//            .where("torrents.id", "=", 256)
////            .first({"id", "name"});
//            .value("name");
////    qDebug() << "SEVENTH :" << f.executedQuery();
////    qDebug() << "id :" << f.value("id") << "; name :" << f.value("name");
//    qDebug() << "name:" << f;
//    qt_noop();

//    auto [ok, g] = m_em.queryBuilder()->from("torrents")
//            .find(256, {"id", "name"});
//    qDebug() << "EIGTH :" << g.executedQuery();
//    qDebug() << "id :" << g.value("id") << "; name :" << g.value("name");
//    qt_noop();

    /* GROUP BY and HAVING */
//    auto h = m_em.queryBuilder()->from("torrents")
//             .groupBy({"status"})
////             .having("status", ">", 10)
////             .having("status", ">", "Paused")
//             .having("status", ">", static_cast<int>(TorrentStatus::Paused))
//             .get({"id", "name", "status"});
//    qDebug() << "NINETH :" << h.executedQuery();
//    while (h.next()) {
//        qDebug() << "id :" << h.value("id") << "; name :" << h.value("name")
//                 << "status :" << h.value("status");
//    }

    /* INSERTs */
//    auto id_i = m_em.queryBuilder()->table("torrents").insertGetId(
//                    {{"name", "first"}, {"progress", 300}, {"eta", 8000000}, {"size", 2048},
//                     {"seeds", 0}, {"total_seeds", 0}, {"leechers", 0}, {"total_leechers", 0},
//                     {"remaining", 1024},
//                     {"added_on", QDateTime().currentDateTime().toString(Qt::ISODate)},
//                     {"hash", "xxxx61defa3daecacfde5bde0214c4a439351d4d"},
//                     {"status", static_cast<int>(TorrentStatus::Stalled)},
//                     {"savepath", "D:/downloads/uTorrent/downloads/_dev1/_videos"}});
//    qDebug() << "TENTH";
//    qDebug() << "last id :" << id_i;
//    qt_noop();

//    const auto id_i = 278;
//    auto [ok_j, j] = m_em.queryBuilder()->table("torrent_previewable_files").insert({
//        {{"torrent_id", id_i}, {"file_index", 0}, {"filepath", "abc.mkv"}, {"size", 2048},
//            {"progress", 10}},
//        {{"torrent_id", id_i}, {"file_index", 1}, {"filepath", "xyz.mkv"}, {"size", 1024},
//            {"progress", 15}}});
//    qDebug() << "ELEVEN :" << j->executedQuery();
//    if (ok_j) {
//        qDebug() << "last id :" << j->lastInsertId()
//                 << "; affected rows :" << j->numRowsAffected();
//    }
//    qt_noop();

//    auto [ok_k, k] = m_em.queryBuilder()->table("torrent_previewable_files").insert({
//        {"torrent_id", id_i}, {"file_index", 2}, {"filepath", "qrs.mkv"}, {"size", 3074},
//        {"progress", 20}});
//    qDebug() << "TWELVE :" << k->executedQuery();
//    if (ok_k) {
//        qDebug() << "last id :" << k->lastInsertId()
//                 << "; affected rows :" << k->numRowsAffected();
//    }
//    qt_noop();

//    const auto id_l = 278;
//    auto [ok_l, l] = m_em.queryBuilder()->table("torrent_previewable_files").insertOrIgnore({
//        {{"torrent_id", id_l}, {"file_index", 2}, {"filepath", "qrs.mkv"}, {"size", 3074},
//            {"progress", 20}},
//        {{"torrent_id", id_l}, {"file_index", 3}, {"filepath", "ghi.mkv"}, {"size", 3074},
//            {"progress", 20}},
//        {{"torrent_id", id_l}, {"file_index", 4}, {"filepath", "def.mkv"}, {"size", 3074},
//            {"progress", 20}},
//    });
//    qDebug() << "TWELVE :" << l->executedQuery();
//    if (ok_l) {
//        qDebug() << "last id :" << l->lastInsertId() // undefined behavior
//                 << "; affected rows :" << l->numRowsAffected();
//    }
//    qt_noop();

    /* UPDATEs */
//    auto [affected_m, m] = m_em.queryBuilder()->table("torrents")
//            .where("id", "=", 277)
//            .update({{"name", "first1"}, {"progress", 350}});
//    qDebug() << "THIRTEEN :" << m.executedQuery();
//    qDebug() << "affected rows :" << affected_m;
//    qt_noop();

//    auto [affected_n, n] = m_em.queryBuilder()->table("torrents")
//            .join("torrent_previewable_files", "torrents.id", "=",
//                  "torrent_previewable_files.torrent_id")
//            .where("torrents.id", "=", 277)
//            .where("torrent_previewable_files.id", "=", 100)
//            .update({{"name", "first4"}, {"torrents.progress", 354},
//                    {"torrent_previewable_files.progress", 15}});
//    qDebug() << "FOURTEEN :" << n.executedQuery();
//    qDebug() << "affected rows :" << affected_n;
//    qt_noop();

    /* EXPRESSIONs */
//    Expression aa("first1");
//    QVariant x;
//    x.setValue(aa);
//    QVariant x {Expression("first1")};
//    QVariant x = Expression("first1");
//    QVariant x = QVariant::fromValue(Expression("first1"));
//    qDebug() << "text :" << x;
//    qDebug() << "type :" << x.type();
//    qDebug() << "typeName :" << x.typeName();
//    qDebug() << "userType :" << x.userType();
//    qt_noop();

//    auto [affected_o, o] = m_em.queryBuilder()->table("torrents")
//            .where("id", "=", 277)
//            .update({{"name", QVariant::fromValue(Expression("first"))}, {"progress", 350}});
////            .update({{"name", x}, {"progress", 350}});
//    qDebug() << "FIFTEEN :" << o.executedQuery();
//    qDebug() << "affected rows :" << affected_o;
//    qt_noop();

    /* INCREMENT / DECREMENT */
//    auto [affected_p, p] = m_em.queryBuilder()->table("torrents")
//            .whereEq("id", 277)
////            .increment("progress", 1);
//            .decrement("progress", 1, {
//                {"seeds", 6}
//            });
//    qDebug() << "SIXTEEN :" << p.executedQuery();
//    qDebug() << "affected rows :" << affected_p;
//    qt_noop();

    /* DELETEs */
//    auto [affected_q, q] = m_em.queryBuilder()->table("torrent_previewable_files")
//            .whereEq("id", 107)
//            .remove();
//    qDebug() << "SEVENTEEN :" << q.executedQuery();
//    qDebug() << "affected rows :" << affected_q;
//    qt_noop();

//    auto [affected_r, r] = m_em.queryBuilder()->table("torrents")
//            .join("torrent_previewable_files", "torrents.id", "=",
//                  "torrent_previewable_files.torrent_id")
//            .whereEq("torrents.id", 277)
//            .whereEq("torrent_previewable_files.id", 106)
//            .remove();
//    qDebug() << "EIGHTEEN :" << r.executedQuery();
//    qDebug() << "affected rows :" << affected_r;
//    qt_noop();

    /* TRUNCATE */
//    auto [ok_s, s] = m_em.queryBuilder()->table("xxx")
//            .truncate();
//    qDebug() << "NINETEEN :" << s.executedQuery();
//    ok_s ? qDebug() << "truncate was successful"
//            : qDebug() << "truncate was unsuccessful";
//    qt_noop();
}
