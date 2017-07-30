/**********************************************************
 * @file   main.cpp
 * @author jan
 * @date   7/30/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include "main.h"
#include "LanguagePickerPlugin.h"

CoreWidgetsPlugin::CoreWidgetsPlugin(QObject* parent)
        :QObject(parent)
{
    m_widgets.append(new LanguagePickerPlugin(this));
}

QList<QDesignerCustomWidgetInterface*> CoreWidgetsPlugin::customWidgets() const
{
    return m_widgets;
}

