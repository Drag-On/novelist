/**********************************************************
 * @file   ProjectViewPlugin.h
 * @author jan
 * @date   8/12/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_PROJECTVIEWPLUGIN_H
#define NOVELIST_PROJECTVIEWPLUGIN_H

#include <QtUiPlugin/QDesignerCustomWidgetInterface>


class ProjectViewPlugin : public QObject, public QDesignerCustomWidgetInterface {
Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    explicit ProjectViewPlugin(QObject* parent = nullptr);

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

#endif //NOVELIST_PROJECTVIEWPLUGIN_H
