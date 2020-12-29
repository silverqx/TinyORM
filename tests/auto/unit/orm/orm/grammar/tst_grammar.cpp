#include <QCoreApplication>
#include <QtTest>

#include "orm/grammar.hpp"

class tst_Grammar : public QObject
{
    Q_OBJECT

public:
    tst_Grammar();
    ~tst_Grammar() = default;

private slots:
    void initTestCase();
    void cleanupTestCase();

    void test_case1();

};

tst_Grammar::tst_Grammar()
{

}

void tst_Grammar::initTestCase()
{

}

void tst_Grammar::cleanupTestCase()
{

}

void tst_Grammar::test_case1()
{
}

QTEST_MAIN(tst_Grammar)

#include "tst_grammar.moc"
