#ifndef CONTACTLISTMODEL_H
#define CONTACTLISTMODEL_H

#include <QAbstractProxyModel>
#include <QIdentityProxyModel>
#include "contactlistfrontmodel.h"

class ContactListModel : public QIdentityProxyModel
{
    Q_OBJECT
public:
    explicit ContactListModel(QObject *parent = 0);

private:
    ContactListFrontModel *m_model;
};

#endif // CONTACTLISTMODEL_H
