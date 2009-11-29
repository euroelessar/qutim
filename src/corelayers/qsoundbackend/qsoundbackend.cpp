#include "qsoundbackend.h"
#include <qsound.h>
#include <QDebug>

void QSoundBackend::playSound(const QString& filename)
{
	if (!QSound::isAvailable()) {
		qWarning() << "Unable to play sound";
	}
	QSound sound (filename);
	sound.play();
}

QStringList QSoundBackend::supportedFormats()
{
	//TODO
	return QStringList() << "wav";
}
