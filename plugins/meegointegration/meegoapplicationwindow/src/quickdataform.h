#ifndef QUICKDATAFORM_H
#define QUICKDATAFORM_H

#include "applicationwindow.h"
#include <qutim/dataforms.h>
#include <qutim/servicemanager.h>
#include <QDeclarativeItem>

namespace MeegoIntegration
{

class ComponentCreator;

class QuickDataForm : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(QVariant item READ item WRITE setItem NOTIFY itemChanged)
public:
    explicit QuickDataForm(QDeclarativeItem *parent = 0);
    
    QVariant item();
    void setItem(const QVariant &item);
    
signals:
    void itemChanged(const QVariant &item);
    
protected:
    void buildContent();
    void updateItem(Ureen::DataItem &item);
	void addGroup(const Ureen::DataItem &item);
    void addLabel(const Ureen::DataItem &item);
    void addItem(const Ureen::DataItem &item);
    QDeclarativeComponent *loadComponent(const QString &name);
    
protected slots:
    void onHeightChanged();

private:
    Ureen::DataItem m_item;
    QString m_basePath;
    Ureen::ServicePointer<ApplicationWindow> m_app;
    QHash<QString, QDeclarativeComponent *> m_componentsCache;
    QHash<QString, QDeclarativeItem *> m_items;
    bool m_hasSubitems;
    QDeclarativeItem *m_column;
    QDeclarativeContext *m_context;
    friend class ComponentCreator;
};

}

#endif // QUICKDATAFORM_H
