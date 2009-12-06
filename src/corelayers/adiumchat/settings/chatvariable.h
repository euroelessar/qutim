#ifndef CHATVARIABLE_H
#define CHATVARIABLE_H

#include <QWidget>
#include "../chatstyle.h"

using namespace AdiumChat;
namespace Core
{
	class ChatVariable
	{
		public:
			//ChatVariable() = 0;
			virtual AdiumChat::StyleVariableType type() = 0;
			virtual const QString &value() = 0;
			virtual void setValue(const QString &value) = 0;
	};
}
Q_DECLARE_INTERFACE(Core::ChatVariable, "org.qutim.core.ChatVariable")

namespace  Core
{
	class ChatBackground : public QWidget, ChatVariable
	{
		Q_OBJECT
		Q_INTERFACES(Core::ChatVariable)

		public:
			ChatBackground(QWidget *parent);
			StyleVariableType type() {return BACKGROUND;}
			const QString &value();
			void setValue(const QString &value);
	};

	class ChatFont : public QWidget, ChatVariable
	{
		Q_OBJECT
		Q_INTERFACES(Core::ChatVariable)

		public:
			ChatFont(QWidget *parent);
			StyleVariableType type() {return FONT;}
			const QString &value();
			void setValue(const QString &value);
	};
	class ChatColor : public QWidget, ChatVariable
	{
		Q_OBJECT
		Q_INTERFACES(Core::ChatVariable)

		public:
			ChatColor(QWidget *parent);
			StyleVariableType type() {return COLOR;}
			const QString &value();
			void setValue(const QString &value);
	};

	class ChatNumeric : public QWidget, ChatVariable
	{
		Q_OBJECT
		Q_INTERFACES(Core::ChatVariable)

		public:
			ChatNumeric(QWidget *parent);
			StyleVariableType type() {return NUMERIC;}
			const QString &value();
			void setValue(const QString &value);
	};
}

#endif // CHATVARIABLE_H
