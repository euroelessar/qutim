#ifndef QSOUNDBACKEND_H
#define QSOUNDBACKEND_H
#include <libqutim/notificationslayer.h>

using namespace qutim_sdk_0_3;

class QSoundBackend : public SoundBackend
{
public:
	virtual void playSound(const QString& filename);
    virtual QStringList supportedFormats();
};

#endif // QSOUNDBACKEND_H
