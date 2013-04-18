#ifndef CONTACTMANAGER_P_H
#define CONTACTMANAGER_P_H

#include "contactmanager.h"
#include <TelepathyQt/ContactManager>

class ContactManagerPrivate
{
public:
    ContactManagerPrivate(ContactManager *q_ptr)
        : q(q_ptr), state(Tp::ContactListStateNone)
    {
    }

    void setContactManager(const Tp::ContactManagerPtr &manager);
    void onStateChanged(Tp::ContactListState state);
    Contact *ensureContact(const Tp::ContactPtr &contact, bool *added);

    ContactManager *q;
    Tp::ContactManagerPtr manager;
    Tp::ContactListState state;
    QHash<QString, Contact*> contacts;
    Account *account;
};

#endif // CONTACTMANAGER_P_H
