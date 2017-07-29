/**********************************************************
 * @file   ProjectViewTest.cpp
 * @author jan
 * @date   7/21/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include <catch.hpp>
#include <QtWidgets/QApplication>
#include <QDebug>
#include "view/ProjectView.h"

using namespace novelist;

class Application final : public QApplication {
Q_OBJECT

public:
    Application(int& argc, char** argv)
            :QApplication(argc, argv) { }

    bool notify(QObject* receiver, QEvent* e) override
    {
        try {
            return QApplication::notify(receiver, e);
        }
        catch (std::exception& e) {
            qDebug() << "Caught exception: " << e.what();
            std::terminate();
        }
        catch (...) {
            qDebug() << "Caught unknown exception";
            std::terminate();
        }

    }
};

#include "ProjectViewTest.moc"

TEST_CASE("ProjectView start", "[.Interactive][View]")
{
    int argc = 0;
    Application app(argc, nullptr);

    ProjectProperties properties{"Foo", "Ernie", Language::en_US};
    ProjectModel model{properties};
    ProjectView projectView(nullptr);
    projectView.setModel(&model);
    projectView.show();

    app.exec();
}