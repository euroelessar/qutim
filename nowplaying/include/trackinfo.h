#ifndef TRACKINFO_H
#define TRACKINFO_H

#include <QString>
#include <QUrl>
#include <QMetaType>

namespace qutim_sdk_0_3 {

namespace nowplaying {

	struct TrackInfo
	{
		QString artist;
		QString title;
		QString album;
		qint64 time;
		qint32 trackNumber;
		QUrl location;
	};

} 
}

Q_DECLARE_METATYPE(qutim_sdk_0_3::nowplaying::TrackInfo)

#endif // TRACKINFO_H
