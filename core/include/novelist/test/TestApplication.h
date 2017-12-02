/**********************************************************
 * @file   TestApplication.h
 * @author jan
 * @date   10/3/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_MACROS_H
#define NOVELIST_MACROS_H

#include <novelist_core_export.h>

/**
 * Define a data-driven section in a catch TEST_CASE
 */
#define DATA_SECTION(name, testfun, ...) \
    SECTION(name) { \
        auto testFun = testfun; \
        __VA_ARGS__ \
    }

/**
 * Defines the test function to run on all data
 */
#define TESTFUN(...) __VA_ARGS__

/**
 * Define a data row and give it a name
 */
#define NAMED_ROW(name, ...) SECTION(name) { testFun(__VA_ARGS__); }

/**
 * Define a data row without a name
 */
#define ROW(...) NAMED_ROW(#__VA_ARGS__, __VA_ARGS__)

#include <QApplication>
#include <QDebug>

namespace novelist {
    class NOVELIST_CORE_EXPORT TestApplication final : public QApplication {
    Q_OBJECT

    public:
        using QApplication::QApplication;

        bool notify(QObject* receiver, QEvent* e) override;
    };
}

#endif //NOVELIST_MACROS_H
