#include <QCoreApplication>
#include <QtTest>

class tst_Grammar : public QObject
{
    Q_OBJECT

private slots:
    void test_case1() const;
};

void tst_Grammar::test_case1() const
{}

QTEST_MAIN(tst_Grammar)

#include "tst_grammar.moc"
