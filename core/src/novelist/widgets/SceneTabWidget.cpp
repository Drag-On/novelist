/**********************************************************
 * @file   SceneTabWidget.cpp
 * @author jan
 * @date   8/6/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include "widgets/SceneTabWidget.h"

namespace novelist
{
    SceneTabWidget::SceneTabWidget(QWidget* parent)
            :QTabWidget(parent)
    {
        setDocumentMode(true);
        setTabsClosable(true);
        setMovable(true);
    }
}