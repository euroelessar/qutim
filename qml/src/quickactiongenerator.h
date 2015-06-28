#ifndef QUTIM_SDK_0_3_QUICKACTIONGENERATOR_H
#define QUTIM_SDK_0_3_QUICKACTIONGENERATOR_H

#include <QObject>

namespace qutim_sdk_0_3 {

class QuickActionGenerator : public QObject
{
	Q_OBJECT
public:
	explicit QuickActionGenerator(QObject *parent = 0);

signals:

public slots:

};

} // namespace qutim_sdk_0_3

#endif // QUTIM_SDK_0_3_QUICKACTIONGENERATOR_H
