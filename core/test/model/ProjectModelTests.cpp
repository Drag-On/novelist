/**********************************************************
 * @file   ProjectModelTests.cpp
 * @author jan
 * @date   17.06.17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
/**********************************************************
 * @file   LanguageTests.cpp
 * @author jan
 * @date   17.06.17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include "main.h"
#include "model/ProjectModel.h"

using namespace novelist;

class ProjectModelTests : public QObject {
Q_OBJECT

private:
    ProjectModel m_model;
    QModelIndex m_firstDocument;

private slots:

    void initTestCase()
    {
        m_firstDocument = m_model.createDocument("foobar");
    }

    void cleanupTestCase()
    {
    }

    void constructor()
    {
        QFETCH(QString, name);
        QFETCH(QString, author);
        QFETCH(QString, lang);

        ProjectModel model(nullptr, name, author, Language(lang));
        QCOMPARE(model.name(), name);
        QCOMPARE(model.author(), author);
        QCOMPARE(model.lang().shortname(), lang);
    }

    void constructor_data()
    {
        QTest::addColumn<QString>("name");
        QTest::addColumn<QString>("author");
        QTest::addColumn<QString>("lang");

        QTest::newRow("foo/bar/en_EN") << "foo" << "bar" << "en_EN";
        QTest::newRow("baz/bam/en_US") << "baz" << "bam" << "en_US";
    }

    void setName()
    {
        QFETCH(QString, name);

        m_model.setName(name);
        QCOMPARE(m_model.name(), name);
    }

    void setName_data()
    {
        QTest::addColumn<QString>("name");

        QTest::newRow("foo") << "foo";
        QTest::newRow("bar") << "bar";
    }

    void setAuthor()
    {
        QFETCH(QString, author);

        m_model.setAuthor(author);
        QCOMPARE(m_model.author(), author);
    }

    void setAuthor_data()
    {
        QTest::addColumn<QString>("author");

        QTest::newRow("foo") << "foo";
        QTest::newRow("bar") << "bar";
    }

    void setLang()
    {
        QFETCH(QString, lang);

        m_model.setLang(Language(lang));
        QCOMPARE(m_model.lang().shortname(), lang);
    }

    void setLang_data()
    {
        QTest::addColumn<QString>("lang");

        QTest::newRow("en_EN") << "en_EN";
        QTest::newRow("de_DE") << "de_DE";
    }

    void createChapter()
    {
        QFETCH(QString, chapter);

        auto oldCount = m_model.rowCount(m_model.projectRoot()->index());
        auto newIdx = m_model.createChapter(m_firstDocument, chapter);
        QCOMPARE(m_model.rowCount(m_model.projectRoot()->index()), oldCount + 1);
        QCOMPARE(m_model.itemFromIndex(newIdx)->text(), chapter);
        QCOMPARE(qvariant_cast<ProjectModel::NodeType>(m_model.itemFromIndex(newIdx)->data(ProjectModel::NodeTypeRole)),
                ProjectModel::NodeType::Chapter);
    }

    void createChapter_data()
    {
        QTest::addColumn<QString>("chapter");

        QTest::newRow("foo") << "foo";
        QTest::newRow("bar") << "bar";
    }

    void createDocument()
    {
        QFETCH(QString, doc);

        auto oldCount = m_model.rowCount(m_model.projectRoot()->index());
        auto newIdx = m_model.createDocument(m_firstDocument, doc);
        QCOMPARE(m_model.rowCount(m_model.projectRoot()->index()), oldCount + 1);
        QCOMPARE(m_model.itemFromIndex(newIdx)->text(), doc);
        QCOMPARE(qvariant_cast<ProjectModel::NodeType>(m_model.itemFromIndex(newIdx)->data(ProjectModel::NodeTypeRole)),
                ProjectModel::NodeType::Document);
    }

    void createDocument_data()
    {
        QTest::addColumn<QString>("doc");

        QTest::newRow("foo") << "foo";
        QTest::newRow("bar") << "bar";
    }
};

TEST_SUITE(ProjectModelTests)

#include "ProjectModelTests.moc"