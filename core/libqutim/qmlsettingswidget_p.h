#ifndef QUTIM_SDK_0_3_QMLSETTINGSWIDGET_P_H
#define QUTIM_SDK_0_3_QMLSETTINGSWIDGET_P_H

#include "settingswidget.h"

namespace qutim_sdk_0_3 {

class QmlSettingsWidget : public SettingsWidget
{
    Q_OBJECT
public:
    explicit QmlSettingsWidget(const QString &name, QWidget *parent = 0);

    void setController(QObject *controller);

protected slots:
    void onModifiedChanged();

protected:
    void loadImpl();
    void saveImpl();
    void cancelImpl();

private:
    QScopedPointer<QObject> m_wrapper;
    QObject *m_object;
};

} // namespace qutim_sdk_0_3

#endif // QUTIM_SDK_0_3_QMLSETTINGSWIDGET_P_H
