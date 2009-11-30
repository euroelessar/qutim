#ifndef BUDDYLISTMANAGER_H
#define BUDDYLISTMANAGER_H

#include <QObject>

struct SendItem;
class BuddyListManager : public QObject
{
	Q_OBJECT
public:
    BuddyListManager(const QString &path);
	QList<SendItem> Load() const;
        void Save (const QList<SendItem> &items);
private:
	QString path;
        QVariant toQVariant(const SendItem &item) const;
        SendItem toSendItem (const QVariant &variant) const;
};

#endif // BUDDYLISTMANAGER_H
