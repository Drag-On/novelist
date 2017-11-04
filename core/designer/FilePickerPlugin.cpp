/**********************************************************
 * @file   FilePickerPlugin.cpp
 * @author jan
 * @date   11/4/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include <widgets/FilePicker.h>
#include "FilePickerPlugin.h"

FilePickerPlugin::FilePickerPlugin(QObject* parent)
        :QObject(parent)
{
}

QString FilePickerPlugin::name() const
{
    return "novelist::FilePicker";
}

QString FilePickerPlugin::group() const
{
    return "Input Widgets";
}

QString FilePickerPlugin::toolTip() const
{
    return "A simple file picker";
}

QString FilePickerPlugin::whatsThis() const
{
    return toolTip();
}

QString FilePickerPlugin::includeFile() const
{
    return "widgets/FilePicker.h";
}

QIcon FilePickerPlugin::icon() const
{
    return QIcon();
}

bool FilePickerPlugin::isContainer() const
{
    return false;
}

QWidget* FilePickerPlugin::createWidget(QWidget* parent)
{
    return new novelist::FilePicker(parent);
}

bool FilePickerPlugin::isInitialized() const
{
    return QDesignerCustomWidgetInterface::isInitialized();
}

void FilePickerPlugin::initialize(QDesignerFormEditorInterface* core)
{
    QDesignerCustomWidgetInterface::initialize(core);
}

QString FilePickerPlugin::domXml() const
{
    return "<ui language=\"c++\"> displayname=\"FilePicker\">\n"
            "    <widget class=\"novelist::FilePicker\" name=\"filePicker\"/>\n"
            "</ui>";
}
