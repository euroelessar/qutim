#ifndef QUTIM_SDK_0_3_QUICKACTIONCONTAINERBASE_H
#define QUTIM_SDK_0_3_QUICKACTIONCONTAINERBASE_H

#include <QObject>
#include <QQmlParserStatus>
#include <qutim/menucontroller.h>
#include <qutim/quickaction.h>

namespace qutim_sdk_0_3 {

class QuickActionContainerBase : public QObject, public ActionHandler, public QQmlParserStatus
{
    Q_OBJECT
    Q_ENUMS(ActionType Filter)
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QObject* controller READ controller WRITE setController NOTIFY controllerChanged)
    Q_PROPERTY(ActionType actionType READ actionType WRITE setActionType NOTIFY actionTypeChanged)
    Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)
public:
    enum ActionType {
        AllActions = -1,
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

    explicit QuickActionContainerBase(QObject *parent = 0);
    ~QuickActionContainerBase();

    void classBegin();
    void componentComplete();

    QObject *controller() const;
    bool visible() const;

    void setController(QObject *controller);
    void setVisible(bool visible);

    ActionType actionType() const;
    void setActionType(ActionType actionType);

signals:
    void controllerChanged(QObject *arg);
    void visibleChanged(bool arg);
    void actionTypeChanged(ActionType actionType);

protected:
    QuickAction *create(QAction *action);
    QuickAction *createSeparator();

    ActionContainer m_container;
    QObject *m_controller;
    bool m_visible;
    bool m_completed;
    ActionType m_actionType;
};

} // namespace qutim_sdk_0_3

#endif // QUTIM_SDK_0_3_QUICKACTIONCONTAINERBASE_H
