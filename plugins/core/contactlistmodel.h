#ifndef CONTACTLISTMODEL_H
#define CONTACTLISTMODEL_H

#include <QAbstractProxyModel>
#include <QIdentityProxyModel>
#include "contactlistfrontmodel.h"

class ContactListModel : public QIdentityProxyModel
{
    Q_OBJECT
    Q_PROPERTY(bool showOffline READ offlineVisibility WRITE setOfflineVisibility NOTIFY offlineVisibilityChanged)
	Q_PROPERTY(QStringList tags READ tags NOTIFY tagsChanged)
	Q_PROPERTY(QStringList filterTags READ filterTags WRITE setFilterTags NOTIFY filterTagsChanged)
public:
    explicit ContactListModel(QObject *parent = 0);

    bool offlineVisibility() const;
    QStringList tags() const;
    QStringList filterTags() const;

    void setOfflineVisibility(bool offlineVisibility);
    void setFilterTags(const QStringList &filterTags);

signals:
    void offlineVisibilityChanged(bool offlineVisibility);
    void tagsChanged(const QStringList &tags);
    void filterTagsChanged(const QStringList &filterTags);

private:
    ContactListFrontModel *m_model;
    bool m_showOffline;
    QStringList m_tags;
    QStringList m_filterTags;
};

#endif // CONTACTLISTMODEL_H
