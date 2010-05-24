#ifndef DATAFORMS_H
#define DATAFORMS_H

#include "localizedstring.h"
#include <QSharedPointer>
#include <QVariant>
#include <QFrame>

namespace qutim_sdk_0_3
{

	class DataItemPrivate;

	class LIBQUTIM_EXPORT DataItem
	{
	public:
		DataItem();
		DataItem(const QString &name, const LocalizedString &title, const QVariant &data);
		DataItem(const LocalizedString &title, const QVariant &data = QVariant());
		DataItem(const DataItem &item);
		~DataItem();
		DataItem &operator=(const DataItem &item);
		QString name() const;
		void setName(const QString &name);
		LocalizedString title() const;
		void setTitle(const LocalizedString &title);
		QVariant data() const;
		void setData(const QVariant &data);
		bool isNull() const;
		QList<DataItem> subitems() const;
		void addSubitem(const DataItem &item);
		bool hasSubitems() const;
		void setMultiple(const DataItem &defaultSubitem, int maxCount = -1);
		bool isMultiple() const;
		int maxCount() const;
		DataItem defaultSubitem() const;
		bool isReadOnly() const;
		void setReadOnly(bool readOnly = true);
		QVariant property(const char *name, const QVariant &def = QVariant()) const;
		template<typename T>
		T property(const char *name, const T &def = T()) const
		{ return qVariantValue<T>(property(name, qVariantFromValue<T>(def))); }
		void setProperty(const char *name, const QVariant &value);
	protected:
		QSharedDataPointer<DataItemPrivate> d;
	};

	class LIBQUTIM_EXPORT AbstractDataWidget
	{
	public:
		virtual DataItem item() const = 0;
		virtual ~AbstractDataWidget() {}
	protected:
		virtual void virtual_hook(int id, void *data);
	};

	class LIBQUTIM_EXPORT AbstractDataForm : public QFrame, public AbstractDataWidget
	{
		Q_OBJECT
	public:
		enum ButtonRole {
			InvalidRole = -1,
			AcceptRole,
			RejectRole,
			DestructiveRole,
			ActionRole,
			HelpRole,
			YesRole,
			NoRole,
			ResetRole,
			ApplyRole,

			NRoles
		};
		Q_DECLARE_FLAGS(ButtonRoles, ButtonRole);
		enum StandardButton {
			NoButton           = 0x00000000,
			Ok                 = 0x00000400,
			Save               = 0x00000800,
			SaveAll            = 0x00001000,
			Open               = 0x00002000,
			Yes                = 0x00004000,
			YesToAll           = 0x00008000,
			No                 = 0x00010000,
			NoToAll            = 0x00020000,
			Abort              = 0x00040000,
			Retry              = 0x00080000,
			Ignore             = 0x00100000,
			Close              = 0x00200000,
			Cancel             = 0x00400000,
			Discard            = 0x00800000,
			Help               = 0x01000000,
			Apply              = 0x02000000,
			Reset              = 0x04000000,
			RestoreDefaults    = 0x08000000,
		};
		Q_DECLARE_FLAGS(StandardButtons, StandardButton);
		struct Button
		{
			LocalizedString name;
			ButtonRole role;
		};
		typedef QList<Button> Buttons;
		static QWidget *get(const DataItem &item, StandardButtons standartButtons = NoButton,
							 const Buttons &buttons = Buttons());
	signals:
		void accepted();
		void rejected();
		void helpRequested();
		void clicked(const QString &name);
	};

	class LIBQUTIM_EXPORT DataFormsBackend : public QObject
	{
		Q_OBJECT
	public:
		virtual QWidget *get(const DataItem &item,
							  AbstractDataForm::StandardButtons standartButtons = AbstractDataForm::NoButton,
							  const AbstractDataForm::Buttons &buttons = AbstractDataForm::Buttons()) = 0;
		static DataFormsBackend *instance();
	};
}

Q_DECLARE_INTERFACE(qutim_sdk_0_3::AbstractDataWidget, "org.qutim.core.AbstractDataWidget");

#endif // DATAFORMS_H
