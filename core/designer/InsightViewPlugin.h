/**********************************************************
 * @file   InsightViewPlugin.h
 * @author jan
 * @date   10/22/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_INSIGHTVIEWPLUGIN_H
#define NOVELIST_INSIGHTVIEWPLUGIN_H

#include <QtUiPlugin/QDesignerCustomWidgetInterface>


class InsightViewPlugin : public QObject, public QDesignerCustomWidgetInterface {
Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    explicit InsightViewPlugin(QObject* parent = nullptr);

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

#endif //NOVELIST_INSIGHTVIEWPLUGIN_H
