#ifndef QUTIM_SDK_0_3_QUICKACTIONCONTAINER_H
#define QUTIM_SDK_0_3_QUICKACTIONCONTAINER_H

#include <QObject>
#include <QQmlParserStatus>
#include <qutim/menucontroller.h>
#include "quickaction.h"

namespace qutim_sdk_0_3 {

class QuickActionContainer : public QObject, public ActionHandler, public QQmlParserStatus
{
    Q_OBJECT
    Q_ENUMS(ActionType Filter)
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QObject* controller READ controller WRITE setController NOTIFY controllerChanged)
    Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    enum ActionType {
        ContactList = ActionTypeContactList,
        ChatButton = ActionTypeChatButton,
        Additional = ActionTypeAdditional,
        Account = ActionTypeAccount,
        Preferences = ActionTypePreferences,
        ContactInfo = ActionTypeContactInfo
    };

    enum Filter {
        Invalid = ActionContainer::Invalid,
        TypeMatch = ActionContainer::TypeMatch,
        TypeMismatch = ActionContainer::TypeMismatch
    };

    explicit QuickActionContainer(QObject *parent = 0);
    ~QuickActionContainer();

    void handleActionAdded(int index, QAction *action);
    void handleActionRemoved(int index);
    void handleActionsCleared();

    void classBegin();
    void componentComplete();

    QObject *controller() const;
    bool visible() const;

    void setController(QObject *controller);
    void setVisible(bool visible);

    int count() const;

public slots:
    QuickAction *item(int index);

signals:
    void controllerChanged(QObject *arg);
    void visibleChanged(bool arg);
    void countChanged(int arg);

    void actionAdded(int index, QuickAction *action);
    void actionRemoved(int index);

private:
    ActionContainer m_container;
    QList<QuickAction *> m_actions;
    QObject *m_controller;
    bool m_visible;
    bool m_completed;
    int m_count;
};

} // namespace qutim_sdk_0_3

#endif // QUTIM_SDK_0_3_QUICKACTIONCONTAINER_H
