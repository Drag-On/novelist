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
#include "ProjectViewPlugin.h"
#include "SceneTabWidgetPlugin.h"
#include "InsightViewPlugin.h"
#include "FilePickerPlugin.h"

CoreWidgetsPlugin::CoreWidgetsPlugin(QObject* parent)
        :QObject(parent)
{
    m_widgets.append(new LanguagePickerPlugin(this));
    m_widgets.append(new FilePickerPlugin(this));
    m_widgets.append(new ProjectViewPlugin(this));
    m_widgets.append(new InsightViewPlugin(this));
    m_widgets.append(new SceneTabWidgetPlugin(this));
}

QList<QDesignerCustomWidgetInterface*> CoreWidgetsPlugin::customWidgets() const
{
    return m_widgets;
}

