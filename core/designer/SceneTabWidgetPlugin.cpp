/**********************************************************
 * @file   SceneTabWidgetPlugin.cpp
 * @author jan
 * @date   8/12/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include "widgets/SceneTabWidget.h"
#include "SceneTabWidgetPlugin.h"

SceneTabWidgetPlugin::SceneTabWidgetPlugin(QObject* parent)
        :QObject(parent)
{
}

QString SceneTabWidgetPlugin::name() const
{
    return "novelist::SceneTabWidget";
}

QString SceneTabWidgetPlugin::group() const
{
    return "Containers";
}

QString SceneTabWidgetPlugin::toolTip() const
{
    return "Tab Widget for showing novelist scenes";
}

QString SceneTabWidgetPlugin::whatsThis() const
{
    return toolTip();
}

QString SceneTabWidgetPlugin::includeFile() const
{
    return "widgets/SceneTabWidget.h";
}

QIcon SceneTabWidgetPlugin::icon() const
{
    return QIcon();
}

bool SceneTabWidgetPlugin::isContainer() const
{
    return true;
}

QWidget* SceneTabWidgetPlugin::createWidget(QWidget* parent)
{
    return new novelist::SceneTabWidget(parent);
}

bool SceneTabWidgetPlugin::isInitialized() const
{
    return QDesignerCustomWidgetInterface::isInitialized();
}

void SceneTabWidgetPlugin::initialize(QDesignerFormEditorInterface* core)
{
    QDesignerCustomWidgetInterface::initialize(core);
}

QString SceneTabWidgetPlugin::domXml() const
{
    return "<ui language=\"c++\"> displayname=\"SceneTabWidget\">\n"
            "    <widget class=\"novelist::SceneTabWidget\" name=\"sceneTabWidget\">\n"
            "       <property name=\"geometry\">\n"
            "           <rect>\n"
            "               <x>0</x>\n"
            "               <y>0</y>\n"
            "               <width>100</width>\n"
            "               <height>100</height>\n"
            "           </rect>\n"
            "       </property>\n"
            "    </widget>\n"
            "    <customwidgets>\n"
            "       <customwidget>\n"
            "           <class>novelist::SceneTabWidget</class>\n"
            "       </customwidget>\n"
            "    </customwidgets>\n"
            "</ui>";
}
