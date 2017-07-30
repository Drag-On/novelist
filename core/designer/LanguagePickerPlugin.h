/**********************************************************
 * @file   LanguagePickerPlugin.h
 * @author jan
 * @date   7/30/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_LANGUAGEPICKERPLUGIN_H
#define NOVELIST_LANGUAGEPICKERPLUGIN_H

#include <QtUiPlugin/QDesignerCustomWidgetInterface>
#include "widgets/LanguagePicker.h"

class LanguagePickerPlugin : public QObject, public QDesignerCustomWidgetInterface {
Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    explicit LanguagePickerPlugin(QObject* parent = nullptr);

    QString name() const override;

    QString group() const override;

    QString toolTip() const override;

    QString whatsThis() const override;

    QString includeFile() const override;

    QIcon icon() const override;

    bool isContainer() const override;

    QWidget* createWidget(QWidget* parent) override;

    bool isInitialized() const override;

    void initialize(QDesignerFormEditorInterface* core) override;

    QString domXml() const override;
};

#endif //NOVELIST_LANGUAGEPICKERPLUGIN_H
