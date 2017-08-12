/**********************************************************
 * @file   ProjectViewPlugin.cpp
 * @author jan
 * @date   8/12/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include "view/ProjectView.h"
#include "ProjectViewPlugin.h"

ProjectViewPlugin::ProjectViewPlugin(QObject* parent)
        :QObject(parent)
{
}

QString ProjectViewPlugin::name() const
{
    return "novelist::ProjectView";
}

QString ProjectViewPlugin::group() const
{
    return "Item Views (Model-Based)";
}

QString ProjectViewPlugin::toolTip() const
{
    return "Specialized tree view for novelist projects";
}

QString ProjectViewPlugin::whatsThis() const
{
    return toolTip();
}

QString ProjectViewPlugin::includeFile() const
{
    return "view/ProjectView.h";
}

QIcon ProjectViewPlugin::icon() const
{
    return QIcon();
}

bool ProjectViewPlugin::isContainer() const
{
    return false;
}

QWidget* ProjectViewPlugin::createWidget(QWidget* parent)
{
    return new novelist::ProjectView(parent);
}

bool ProjectViewPlugin::isInitialized() const
{
    return QDesignerCustomWidgetInterface::isInitialized();
}

void ProjectViewPlugin::initialize(QDesignerFormEditorInterface* core)
{
    QDesignerCustomWidgetInterface::initialize(core);
}

QString ProjectViewPlugin::domXml() const
{
    return "<ui language=\"c++\"> displayname=\"ProjectView\">\n"
            "    <widget class=\"novelist::ProjectView\" name=\"projectView\"/>\n"
            "    <customwidgets>\n"
            "        <customwidget>\n"
            "            <class>novelist::ProjectView</class>\n"
            "        </customwidget>\n"
            "    </customwidgets>\n"
            "</ui>";
}
