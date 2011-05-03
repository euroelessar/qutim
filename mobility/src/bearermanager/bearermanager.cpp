#include "bearermanager.h"
#include <QNetworkConfigurationManager>

BearerManager::BearerManager(QObject *parent) :
	QObject(parent),
	m_confManager(new QNetworkConfigurationManager(this))
{
	Q_UNUSED(QT_TRANSLATE_NOOP("Service", "BearerManager"));
}
