#ifndef SHORTCUTSETTINGS_H
#define SHORTCUTSETTINGS_H

#include <QObject>
#include <qutim/startupmodule.h>

namespace Core
{

	class ShortcutSettings : public QObject
	{
		Q_OBJECT
	public:
		explicit ShortcutSettings(QObject *parent = 0);
	};

}
#endif // SHORTCUTSETTINGS_H
