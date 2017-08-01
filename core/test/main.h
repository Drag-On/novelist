/**********************************************************
 * @file   main.h.h
 * @author jan
 * @date   8/1/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_MAIN_H_H
#define NOVELIST_MAIN_H_H

#include <QApplication>
#include <QDebug>

class TestApplication final : public QApplication {
Q_OBJECT

public:
    using QApplication::QApplication;

    bool notify(QObject* receiver, QEvent* e) override;
};

#endif //NOVELIST_MAIN_H_H
