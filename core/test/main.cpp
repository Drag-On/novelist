/**********************************************************
 * @file   main.cpp
 * @author jan
 * @date   16.06.17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include <iostream>
#include "main.h"

class SampleTestSuite : public QObject
{
Q_OBJECT

private Q_SLOTS:
    void test1()
    {
        QVERIFY(true);
    }

    void test2()
    {
        QVERIFY(false);
    }
};

TEST_SUITE(SampleTestSuite)

#include "main.moc"

int main(int argc, char* argv[])
{
    return TestCollector::runAll(argc, argv);
}