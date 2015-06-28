#ifndef QUTIM_SDK_0_3_QUICKTHEMEMANAGER_H
#define QUTIM_SDK_0_3_QUICKTHEMEMANAGER_H

#include <QObject>

namespace qutim_sdk_0_3 {

class QuickThemeManager : public QObject
{
	Q_OBJECT
public:
	explicit QuickThemeManager(QObject *parent = 0);

public slots:
	QString path(const QString &category, const QString &themeName);
	QStringList list(const QString &category);

};

} // namespace qutim_sdk_0_3

#endif // QUTIM_SDK_0_3_QUICKTHEMEMANAGER_H
