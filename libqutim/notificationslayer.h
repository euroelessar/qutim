#ifndef NOTIFICATIONSLAYER_H
#define NOTIFICATIONSLAYER_H

#include <QObject>
#include <QPointer>

namespace qutim_sdk_0_3
{
	enum NotificationType
	{
		//TODO
		System,
	};

	struct NotificationsLayerPrivate;
	class PopupBackend : public QObject
	{
		virtual void show(NotificationType type,
						  QObject *sender,
						  const QString &title,
						  const QString &body
						  ) = 0;
	};

	class SoundBackend : public QObject
	{
		Q_OBJECT
	public:
		virtual void playSound (const QString &filename) = 0;
	protected:
		virtual void virtual_hook(int type, void *data());
	};

	class SoundThemeBackend : public QObject
	{
		//TODO
		Q_OBJECT
	public:
		virtual bool loadTheme(const QString &path) = 0;
	};

	class SoundThemeProvider : public QObject
	{
		Q_OBJECT
	public:
		virtual bool init(const QString &path) = 0;
		virtual void setSound(NotificationType sound, const QString &file) = 0;
		virtual QString sound(NotificationType sound) = 0;
	};

	class NotificationsLayer : public QObject
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
