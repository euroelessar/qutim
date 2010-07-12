#ifndef JSONHISTORY_H
#define JSONHISTORY_H

#include "libqutim/history.h"
#include <QDir>
#include <QPointer>

using namespace qutim_sdk_0_3;

namespace Core
{
	class HistoryWindow;
	class JsonHistory : public History
	{
		Q_OBJECT
	public:
		JsonHistory();
		virtual ~JsonHistory();
		uint findEnd(QFile &file);
		virtual void store(const Message &message);
		virtual MessageList read(const ChatUnit *unit, const QDateTime &from, const QDateTime &to, int max_num);
		virtual void showHistory(const ChatUnit *unit);
		static QString quote(const QString &str);
		static QString unquote(const QString &str);
	private slots:
		void onHistoryActionTriggered(QObject *object);
	private:
		QString getFileName(const Message &message) const;
		QDir getAccountDir(const ChatUnit *unit) const;
		QPointer<HistoryWindow> m_history_window;
	};
}

#endif // JSONHISTORY_H
