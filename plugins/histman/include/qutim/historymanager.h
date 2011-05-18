#ifndef HISTORYMANAGER_H
#define HISTORYMANAGER_H

#include <QIcon>
#include <QString>
#include <QDateTime>
#include <QList>
#include <QPair>
#include <QWidget>
#include <qutim/message.h>

namespace HistoryManager
{
	const char * const EventImporters = "Plugin/HistoryManager/HistoryImporter";
	const char * const EventExporters = "Plugin/HistoryManager/HistoryExporter";

//	struct Message
//	{
//		QDateTime time;
//		QString text;
//		qint8 type;
//		bool in;
//	};

	typedef QMap<qint64,   qutim_sdk_0_3::MessageList> Contact;
	typedef QHash<QString, Contact>                    Account;
	typedef QHash<QString, Account>                    Protocol;

	typedef QPair<QWidget *, QWidget *> ConfigWidget;

	class DataBaseInterface
	{
	protected:
		virtual ~DataBaseInterface() {}
	public:
		virtual void appendMessage(const qutim_sdk_0_3::Message &message) = 0;
		virtual void setProtocol(const QString &protocol) = 0;
		virtual void setAccount(const QString &account) = 0;
		virtual void setContact(const QString &contact) = 0;
		virtual void setMaxValue(int max) = 0;
		virtual void setValue(int value) = 0;
		virtual ConfigWidget createAccountWidget(const QString &protocol) = 0;
	};

	class HistoryImporter
	{
	public:
		virtual ~HistoryImporter() {}
		inline void setDataBase(DataBaseInterface *data_base) { m_data_base = data_base; }
		inline void setCharset(const QByteArray &charset) { m_charset = charset; }
		virtual void loadMessages(const QString &path) = 0;
		virtual bool validate(const QString &path) = 0;
		virtual QString name() = 0;
		virtual QIcon icon() = 0;
		virtual QList<ConfigWidget> config() { return QList<ConfigWidget>(); }
		virtual bool useConfig() { return true; }
		virtual bool needCharset() { return false; }
		virtual QString additionalInfo() { return QString(); }
		virtual bool chooseFile() { return false; }
	protected:
		inline void appendMessage(const qutim_sdk_0_3::Message &message) { m_data_base->appendMessage(message); }
		inline void setProtocol(const QString &protocol) { m_data_base->setProtocol(protocol); }
		inline void setAccount(const QString &account) { m_data_base->setAccount(account); }
		inline void setContact(const QString &contact) { m_data_base->setContact(contact); }
		inline void setMaxValue(int max) { m_data_base->setMaxValue(max); }
		inline void setValue(int value) { m_data_base->setValue(value); }
		inline ConfigWidget createAccountWidget(const QString &protocol) Q_REQUIRED_RESULT
		{ return m_data_base->createAccountWidget(protocol); }
		inline QByteArray charset() { return m_charset; }
	private:
		friend class DataBaseInterface;
		DataBaseInterface *m_data_base;
		QByteArray m_charset;
	};

	class HistoryExporter
	{
	public:
		virtual ~HistoryExporter() {}
		virtual void writeMessages(const QHash<QString, Protocol> &data) = 0;
		virtual QString name() = 0;
		virtual QIcon icon() = 0;
	private:
		friend class DataBaseInterface;
		DataBaseInterface *m_data_base;
	};
}

Q_DECLARE_INTERFACE(HistoryManager::HistoryImporter, "org.qutim.HistoryManager.HistoryImporter")
Q_DECLARE_INTERFACE(HistoryManager::HistoryExporter, "org.qutim.HistoryManager.HistoryExporter")

#endif // HISTORYMANAGER_H
