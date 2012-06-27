#ifndef MEEGOINTEGRATION_QUICKICONLOADER_H
#define MEEGOINTEGRATION_QUICKICONLOADER_H

#include <qutim/iconloader.h>

namespace MeegoIntegration {

class QuickIconLoader : public qutim_sdk_0_3::IconLoader
{
    Q_OBJECT
public:
    explicit QuickIconLoader();
    
    virtual QIcon doLoadIcon(const QString &name);
	virtual QMovie *doLoadMovie(const QString &name);
	virtual QString doIconPath(const QString &name, uint iconSize);
	virtual QString doMoviePath(const QString &name, uint iconSize);
};

} // namespace MeegoIntegration

#endif // MEEGOINTEGRATION_QUICKICONLOADER_H
