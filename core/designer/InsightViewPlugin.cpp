/**********************************************************
 * @file   InsightViewPlugin.cpp
 * @author jan
 * @date   10/22/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include <view/InsightView.h>
#include "InsightViewPlugin.h"

InsightViewPlugin::InsightViewPlugin(QObject* parent)
        :QObject(parent)
{
}

QString InsightViewPlugin::name() const
{
    return "novelist::InsightView";
}

QString InsightViewPlugin::group() const
{
    return "Item Views (Model-Based)";
}

QString InsightViewPlugin::toolTip() const
{
    return "Specialized table view for insights in novelist";
}

QString InsightViewPlugin::whatsThis() const
{
    return toolTip();
}

QString InsightViewPlugin::includeFile() const
{
    return "view/InsightView.h";
}

QIcon InsightViewPlugin::icon() const
{
    return QIcon();
}

bool InsightViewPlugin::isContainer() const
{
    return false;
}

QWidget* InsightViewPlugin::createWidget(QWidget* parent)
{
    return new novelist::InsightView(parent);
}

bool InsightViewPlugin::isInitialized() const
{
    return QDesignerCustomWidgetInterface::isInitialized();
}

void InsightViewPlugin::initialize(QDesignerFormEditorInterface* core)
{
    QDesignerCustomWidgetInterface::initialize(core);
}

QString InsightViewPlugin::domXml() const
{
    return "<ui language=\"c++\"> displayname=\"InsightView\">\n"
            "    <widget class=\"novelist::InsightView\" name=\"insightView\"/>\n"
            "</ui>";
}
