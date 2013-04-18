#include "contactlistmodel.h"

ContactListModel::ContactListModel(QObject *parent) :
    QIdentityProxyModel(parent), m_model(new ContactListFrontModel(this))
{
    setSourceModel(m_model);
}
