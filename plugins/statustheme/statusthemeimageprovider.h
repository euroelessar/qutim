#ifndef STATUSTHEMEIMAGEPROVIDER_H
#define STATUSTHEMEIMAGEPROVIDER_H

#include <QQuickImageProvider>

class StatusThemeImageProvider : public QQuickImageProvider
{
    Q_DISABLE_COPY(StatusThemeImageProvider)
public:
    StatusThemeImageProvider();
    ~StatusThemeImageProvider();

    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);

private:
    QMap<int, QString> m_baseStatuses;
    QMap<QString, QString> m_statuses;
};

#endif // STATUSTHEMEIMAGEPROVIDER_H

