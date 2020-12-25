#include <QtTest>

#include "orm/grammar.h"

class tst_Grammar : public QObject
{
    Q_OBJECT

public:
    tst_Grammar();
    ~tst_Grammar();

private slots:
    void initTestCase();
    void cleanupTestCase();

    void test_case1();

};

tst_Grammar::tst_Grammar()
{

}

tst_Grammar::~tst_Grammar()
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

QTEST_APPLESS_MAIN(tst_Grammar)

#include "tst_grammar.moc"
