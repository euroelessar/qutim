#ifndef BEARERMANAGER_H
#define BEARERMANAGER_H

#include <QObject>

class QNetworkConfigurationManager;
class BearerManager : public QObject
{
    Q_OBJECT
	Q_CLASSINFO("Service", "Bearer")
	Q_CLASSINFO("Uses", "SettingsLayer")
public:
    explicit BearerManager(QObject *parent = 0);
private:
	QNetworkConfigurationManager *m_confManager;
};

#endif // BEARERMANAGER_H
