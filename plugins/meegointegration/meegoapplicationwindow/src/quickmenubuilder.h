#ifndef MEEGOINTEGRATION_QUICKMENUBUILDER_H
#define MEEGOINTEGRATION_QUICKMENUBUILDER_H

#include <QObject>
#include <QDeclarativeItem>
#include <qutim/menucontroller.h>

namespace MeegoIntegration {

class QuickMenuBuilder;

class QuickActionFilter : public QObject, public qutim_sdk_0_3::ActionHandler
{
    Q_OBJECT
public:
    QuickActionFilter(QuickMenuBuilder *parent);
    ~QuickActionFilter();
    
    virtual void actionAdded(QAction *action, int index);
	virtual void actionRemoved(int index);
	virtual void actionsCleared();
    
private slots:
    void onActionChanged();
    
private:
    QuickMenuBuilder *m_builder;
    QList<QAction*> m_actions;
    QList<QAction*> m_visibleActions;
};

class QuickMenuBuilder : public QObject, public qutim_sdk_0_3::ActionHandler
{
    Q_OBJECT
    Q_PROPERTY(qutim_sdk_0_3::MenuController *controller READ controller WRITE setController NOTIFY controllerChanged)
    Q_PROPERTY(QDeclarativeItem *menu READ menu WRITE setMenu NOTIFY menuChanged)
public:
    explicit QuickMenuBuilder(QObject *parent = 0);
    ~QuickMenuBuilder();
    
    qutim_sdk_0_3::MenuController *controller() const;
    void setController(qutim_sdk_0_3::MenuController *controller);
    QDeclarativeItem *menu() const;
    void setMenu(QDeclarativeItem *menu);
    
    virtual void actionAdded(QAction *action, int index);
	virtual void actionRemoved(int index);
	virtual void actionsCleared();
    
public slots:
    
protected:
    void setItemParent(QDeclarativeItem *item, QDeclarativeItem *menu);
    
signals:
    void controllerChanged(qutim_sdk_0_3::MenuController *controller);
    void menuChanged(QDeclarativeItem *menu);
    
private:
    qutim_sdk_0_3::MenuController *m_controller;
    qutim_sdk_0_3::ActionContainer m_container;
    QDeclarativeItem *m_menu;
    QDeclarativeComponent *m_delegate;
    QList<QDeclarativeItem *> m_items;
};

} // namespace MeegoIntegration

#endif // MEEGOINTEGRATION_QUICKMENUBUILDER_H
