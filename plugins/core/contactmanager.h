#ifndef CONTACTMANAGER_H
#define CONTACTMANAGER_H

#include "contact.h"

class Account;
class ContactManagerPrivate;

class ContactManager : public QObject
{
    Q_OBJECT
public:
    ~ContactManager();

    QList<Contact *> allKnownContacts() const;

signals:
    void allKnownContactsChanged(const QList<Contact*> &contactsAdded,
                                 const QList<Contact*> &contactsRemoved);
    
private:
    explicit ContactManager(Account *parent);

    friend class ContactManagerPrivate;
    friend class Account;
    QScopedPointer<ContactManagerPrivate> d;
};

QML_DECLARE_TYPE(ContactManager)

#endif // CONTACTMANAGER_H
