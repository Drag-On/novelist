/**********************************************************
 * @file   LanguageTests.cpp
 * @author jan
 * @date   17.06.17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include "main.h"
#include "lang/Language.h"

using namespace novelist;

class LanguageTests : public QObject
{
    Q_OBJECT

private slots:
    void shortname()
    {
        QFETCH(QString, shortname);

        Language lang(shortname);
        QCOMPARE(lang.shortname(), shortname);
    }

    void shortname_data()
    {
        QTest::addColumn<QString>("shortname");

        QTest::newRow("english (uk)") << "en_EN";
        QTest::newRow("english (us)") << "en_US";
        QTest::newRow("german") << "de_DE";
    }
};

TEST_SUITE(LanguageTests)

#include "LanguageTests.moc"