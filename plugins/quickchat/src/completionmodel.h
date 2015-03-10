#ifndef QUICKCHAT_COMPLETIONMODEL_H
#define QUICKCHAT_COMPLETIONMODEL_H

#include <QAbstractItemModel>

namespace QuickChat {

class CompletionModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject *model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(bool valid READ isValid NOTIFY modelChanged)

public:
    explicit CompletionModel(QObject *parent = 0);

    bool isValid() const;

    QObject *model() const;
    void setModel(QObject *model);

public slots:
    QStringList suggestions(const QString &prefix);

signals:
    void modelChanged(QAbstractItemModel *model);

private:
    QAbstractItemModel *m_model;
};

} // namespace QuickChat

#endif // QUICKCHAT_COMPLETIONMODEL_H
