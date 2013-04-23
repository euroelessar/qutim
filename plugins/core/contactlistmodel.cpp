#include "contactlistmodel.h"

ContactListModel::ContactListModel(QObject *parent) :
    QIdentityProxyModel(parent), m_model(new ContactListFrontModel(this))
{
    setSourceModel(m_model);
    connect(m_model, &ContactListFrontModel::offlineVisibilityChanged,
            this, &ContactListModel::offlineVisibilityChanged);
    connect(m_model, &ContactListFrontModel::tagsChanged,
            this, &ContactListModel::tagsChanged);
    connect(m_model, &ContactListFrontModel::filterTagsChanged,
            this, &ContactListModel::filterTagsChanged);
}

bool ContactListModel::offlineVisibility() const
{
    return m_model->offlineVisibility();
}

QStringList ContactListModel::tags() const
{
    return m_model->tags();
}

QStringList ContactListModel::filterTags() const
{
    return m_model->filterTags();
}

void ContactListModel::setOfflineVisibility(bool offlineVisibility)
{
    m_model->setOfflineVisibility(offlineVisibility);
}

void ContactListModel::setFilterTags(const QStringList &filterTags)
{
    m_model->setFilterTags(filterTags);
}
