#ifndef CONTACT_H
#define CONTACT_H

#include <QObject>
#include <qqml.h>
#include <QStringList>
#include "presence.h"

class Account;
class ContactManager;
class ContactManagerPrivate;
class ContactPrivate;

class Contact : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id CONSTANT)
    Q_PROPERTY(QString alias READ alias NOTIFY aliasChanged)
    Q_PROPERTY(QStringList groups READ groups NOTIFY groupsChanged)
    Q_PROPERTY(Presence presence READ presence NOTIFY presenceChanged)
    Q_PROPERTY(QString avatar READ avatar NOTIFY avatarChanged)
    Q_PROPERTY(bool inList READ isInList NOTIFY inListChanged)
public:
    ~Contact();

    Account *account() const;
    
    QString id() const;
    QString alias() const;
    QStringList groups() const;
    Presence presence() const;
    bool isInList() const;
    QString avatar() const;

signals:
    void aliasChanged(const QString &alias);
    void groupsChanged(const QStringList &groups, const QStringList &previous);
    void addedToGroup(const QString &group);
    void removedFromGroup(const QString &group);
    void presenceChanged(const Presence &presence, const Presence &previous);
    void inListChanged(bool inList);
    void avatarChanged(const QString &avatar);

private:
    Contact(ContactPrivate &priv, Account *parent);
    explicit Contact(Account *parent);

    friend class ContactManager;
    friend class ContactManagerPrivate;
    friend class ContactPrivate;
    QScopedPointer<ContactPrivate> d;
};

QML_DECLARE_TYPE(Contact)

#endif // CONTACT_H
