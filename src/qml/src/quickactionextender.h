#ifndef QUTIM_SDK_0_3_QUICKACTIONEXTENDER_H
#define QUTIM_SDK_0_3_QUICKACTIONEXTENDER_H

#include <qutim/menucontroller.h>
#include <QQmlListProperty>

namespace qutim_sdk_0_3 {

class QuickActionExtender : public MenuController
{
	Q_OBJECT
	Q_PROPERTY(qutim_sdk_0_3::MenuController* original READ original WRITE setOriginal NOTIFY originalChanged)
	Q_PROPERTY(QQmlListProperty<QObject> actions READ actions DESIGNABLE false)

public:
	explicit QuickActionExtender(QObject *parent = 0);

	qutim_sdk_0_3::MenuController *original() const;
	void setOriginal(qutim_sdk_0_3::MenuController *original);

	QQmlListProperty<QObject> actions();

signals:
	void originalChanged(qutim_sdk_0_3::MenuController *original);

private:
	qutim_sdk_0_3::MenuController *m_original;
};

} // namespace qutim_sdk_0_3

#endif // QUTIM_SDK_0_3_QUICKACTIONEXTENDER_H
