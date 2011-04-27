#ifndef RESOURCEICONLOADER_H
#define RESOURCEICONLOADER_H
#include <qutim/notificationslayer.h>
#include <QStringList>
#include "3rdparty/xqvibra/xqvibra.h"

namespace Symbian
{
using namespace qutim_sdk_0_3;

class Vibration : public qutim_sdk_0_3::PopupBackend
{
public:
	Vibration();
	~Vibration();

	virtual void show(qutim_sdk_0_3::Notifications::Type type, QObject *sender,
					  const QString &body, const QVariant &data);
private:
	XQVibra *m_vibra;
};

} //namespace Embedded

#endif // RESOURCEICONLOADER_H
