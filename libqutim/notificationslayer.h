#ifndef NOTIFICATIONSLAYER_H
#define NOTIFICATIONSLAYER_H

#include "libqutim_global.h"
#include <QPointer>

namespace qutim_sdk_0_3
{
	enum NotificationType
	{
		//TODO
		System,
	};

	struct NotificationsLayerPrivate;
	class LIBQUTIM_EXPORT PopupBackend : public QObject
	{
		Q_OBJECT
	public:
		virtual void show(NotificationType type,
						  QObject *sender,
						  const QString &title,
						  const QString &body
						  ) = 0;
	};

	class LIBQUTIM_EXPORT SoundBackend : public QObject
	{
		Q_OBJECT
	public:
		virtual void playSound (const QString &filename) = 0;
	protected:
		virtual void virtual_hook(int type, void *data);
	};

	class LIBQUTIM_EXPORT SoundThemeBackend : public QObject
	{
		//TODO
		Q_OBJECT
	public:
		virtual bool loadTheme(const QString &path) = 0;
	};

	class LIBQUTIM_EXPORT SoundThemeProvider : public QObject
	{
		Q_OBJECT
	public:
		virtual bool init(const QString &path) = 0;
		virtual void setSound(NotificationType sound, const QString &file) = 0;
		virtual QString sound(NotificationType sound) = 0;
	};

	class LIBQUTIM_EXPORT NotificationsLayer : public QObject
	{
		Q_OBJECT
	public:
		virtual void sendSimpleNotification(const QString &title,const QString &body = QString());
		virtual void sendNotification(NotificationType type,
									QObject *sender,
									const QString &title,
									const QString &body = QString()
									);
	protected:
		NotificationsLayer();
		virtual ~NotificationsLayer();
	private:
		NotificationsLayerPrivate *p;
	};

}

#endif // NOTIFICATIONSLAYER_H
