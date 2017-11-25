/**********************************************************
 * @file   TestApplication.cpp
 * @author jan
 * @date   11/25/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include "test/TestApplication.h"

namespace novelist {
    bool TestApplication::notify(QObject* receiver, QEvent* e)
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
}