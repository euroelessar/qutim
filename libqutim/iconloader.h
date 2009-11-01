#ifndef ICONLOADER_H
#define ICONLOADER_H

#include "libqutim_global.h"

class QIcon;
class QMovie;

namespace qutim_sdk_0_3
{
	class IconLoader : public QObject
	{
		Q_OBJECT
	public:
		enum StdSize
		{
			StdSize16,
			StdSize22,
			StdSize32,
			StdSize28,
			StdSize64,
			StdSize128
		};
		IconLoader();
		virtual ~IconLoader();
		static IconLoader *instance();
		virtual QIcon loadIcon(const QString &name) = 0;
		virtual QMovie *loadMovie(const QString &name) = 0;
		virtual QString iconPath(const QString &name, StdSize iconSize) = 0;
		virtual QString moviePath(const QString &name, StdSize iconSize) = 0;
	};
}

#endif // ICONLOADER_H
