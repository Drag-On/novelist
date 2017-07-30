/**********************************************************
 * @file   LanguagePickerPlugin.cpp
 * @author jan
 * @date   7/30/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include "LanguagePickerPlugin.h"

LanguagePickerPlugin::LanguagePickerPlugin(QObject* parent)
        :QObject(parent)
{
}

QString LanguagePickerPlugin::name() const
{
    return "novelist::LanguagePicker";
}

QString LanguagePickerPlugin::group() const
{
    return "Input Widgets";
}

QString LanguagePickerPlugin::toolTip() const
{
    return "ComboBox allowing to pick a novel language";
}

QString LanguagePickerPlugin::whatsThis() const
{
    return toolTip();
}

QString LanguagePickerPlugin::includeFile() const
{
    return "widgets/LanguagePicker.h";
}

QIcon LanguagePickerPlugin::icon() const
{
    return QIcon{};
}

bool LanguagePickerPlugin::isContainer() const
{
    return false;
}

QWidget* LanguagePickerPlugin::createWidget(QWidget* parent)
{
    return new novelist::LanguagePicker(parent);
}

bool LanguagePickerPlugin::isInitialized() const
{
    return QDesignerCustomWidgetInterface::isInitialized();
}

void LanguagePickerPlugin::initialize(QDesignerFormEditorInterface* core)
{
    QDesignerCustomWidgetInterface::initialize(core);
}

QString LanguagePickerPlugin::domXml() const
{
    return "<ui language=\"c++\"> displayname=\"LanguagePicker\">\n"
            "    <widget class=\"novelist::LanguagePicker\" name=\"languagePicker\"/>\n"
            "    <customwidgets>\n"
            "        <customwidget>\n"
            "            <class>novelist::LanguagePicker</class>\n"
            "        </customwidget>\n"
            "    </customwidgets>\n"
            "</ui>";
}