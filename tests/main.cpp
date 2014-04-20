#include <QtTest/QtTest>

class TestMain: public QObject
{
    Q_OBJECT
private slots:
    void toUpper();
};

void TestMain::toUpper()
{
    QString str = "Hello";
    QCOMPARE(str.toUpper(), QString("HELLO"));
}

QTEST_MAIN(TestMain)
#include "main.moc"
