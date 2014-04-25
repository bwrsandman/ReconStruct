#include <QtTest/QtTest>
#include <datatypestr.h>
#include <datatypecustom.h>

class TestReconStruct: public QObject
{
    Q_OBJECT
private:
    QByteArray data;
private slots:
    void initTestCase();
    void testStr();
    void testCustomSizeOne();
    void testCustomSizeTwo();
};


void TestReconStruct::initTestCase()
{
    data = QByteArray::fromHex("48 45 4C 4C 4F 20 "
                               "57 4F 52 4C 44 21");
}


void TestReconStruct::testStr()
{
    uint start = 0;
    DataTypeStr interpreter("Test", 5);
    QCOMPARE(interpreter.format(data, start), QString("HELLO"));
    QCOMPARE(start, 5u);
}

void TestReconStruct::testCustomSizeOne()
{
    // Create str interpreter with size 5
    uint start = 0;
    DataTypeCustom interpreter("Test Custom", 1);
    interpreter.add(new DataTypeStr("hello", 5));
    interpreter.add(new DataTypeStr("space", 1));
    interpreter.add(new DataTypeStr("world", 5));
    QString str = interpreter.format(data, start);
    QCOMPARE(str, QString("HELLO WORLD"));
    QCOMPARE(start, 11u);
}

void TestReconStruct::testCustomSizeTwo()
{
    // Create str interpreter with size 5
    uint start = 0;
    DataTypeCustom interpreter("Test Custom 2", 2);
    interpreter.add(new DataTypeStr("Test String Word", 5));
    interpreter.add(new DataTypeStr("Test String Special", 1));
    QString str = interpreter.format(data, start);
    QCOMPARE(str, QString("HELLO WORLD!"));
    QCOMPARE(start, 12u);
}


QTEST_MAIN(TestReconStruct)
#include "testreconstruct.moc"
