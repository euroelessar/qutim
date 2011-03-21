#ifndef MMENU_H
#define MMENU_H

#include <QObject>

namespace MacIntegration
{
	class MMenu : public QObject
	{
		Q_OBJECT
		Q_CLASSINFO("Service", "MainMenu")
		Q_CLASSINFO("Uses", "ContactList")
		public:
			MMenu();
			~MMenu();
	};
}

#endif // MMENU_H
