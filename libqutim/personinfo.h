#ifndef PERSONINFO_H
#define PERSONINFO_H

#include "libqutim_global.h"
#include "localizedstring.h"
#include <QSharedDataPointer>
#include <QMetaType>

namespace qutim_sdk_0_3
{
	class PersonInfoData;

	class LIBQUTIM_EXPORT PersonInfo
	{
	public:
		PersonInfo(const char *name = 0, const char *task = 0,
				   const QString &email = QString(), const QString &web = QString());
		PersonInfo(const PersonInfo &other);
		~PersonInfo();
		PersonInfo &operator =(const PersonInfo &other);
		PersonInfo &setName(const char *name);
		PersonInfo &setTask(const char *task);
		PersonInfo &setEmail(const QString &email);
		PersonInfo &setWeb(const QString &web);
		LocalizedString name() const;
		LocalizedString task() const;
		const QString &email() const;
		const QString &web() const;
	private:
		QSharedDataPointer<PersonInfoData> d;
	public:
		typedef PersonInfoData Data;
		Data *data();
	};
	/*
	 * Encoding is utf-8, language russian
	 * PersonInfo author = PersonInfo(QT_TRANSLATE_NOOP("Author", "Ivan Sidoroff"),
	 *								  QT_TRANSLATE_NOOP("Task", "Plugin author"),
	 *								  "v.sidorov@proger.ru",
	 *								  "http://proger.ru/v.sidorov");
	 * author.name(); // "Иван Сидоров"
	 * author.task(); // "Автор плагина"
	 */
}

Q_DECLARE_METATYPE(qutim_sdk_0_3::PersonInfo)

#endif // PERSONINFO_H
