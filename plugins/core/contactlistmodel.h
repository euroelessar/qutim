#ifndef CONTACTLISTMODEL_H
#define CONTACTLISTMODEL_H

#include <QAbstractProxyModel>
#include <QSortFilterProxyModel>

class ContactListModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit ContactListModel(QObject *parent = 0);
    
signals:
    
public slots:
    
};

#endif // CONTACTLISTMODEL_H
