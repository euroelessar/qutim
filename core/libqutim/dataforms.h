/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/
#ifndef DATAFORMS_H
#define DATAFORMS_H

#include "localizedstring.h"
#include <QSharedPointer>
#include <QVariant>
#include <QFrame>
#include <QDateTime>
#include <QDate>
#include <QPixmap>
#include <QImage>
#include <QIcon>

class QValidator;

namespace qutim_sdk_0_3
{

class DataItemPrivate;

/**
 @brief The DataItem class specifies one field or a group of fields in a \ref AbstractDataForm "data form".

 The DataItem class is part qutIM's DataForms API and describes appearance and behaviour of a group of fields in
 a \ref AbstractDataForm "data form" if it \ref hasSubitems() "has subitems" or
 if \ref allowModifySubitems() "allowed to modify its subitems".
 Otherwise, DataItem describes appearance and behaviour of one field in a \ref AbstractDataForm "data form".

 A DataItem object can have name() attribute by which you can access this object later using subitem().
 Use \ref setName() to set this property.

 Specifying various attributes and properties, you able to change appearance and behaviour of fields in
 a \ref AbstractDataForm "data form". \ref data() is the primary attribute, defines the default field value
 and can be specified using setData() or in the constructors.
 The title() attribute specifes the field title; you can set it using setTitle().
 A DataItem object also may contain dynamic properties which are manipulated using \ref property()
 to read them and \ref setProperty() to write them.
 \note If you set a property for an item, that property would be automatically set for all subitems of the item.
 That behaviour can be very useful when you need to define a default value for some property. For instance, if you
 would set read-only flag for an item, subitems of that item also would have read-only flag.
 Nevertheless, you can always override any property for a subitem.

 The following types of \ref data() and the dynamic properties are supported by DataForms API:
 -# \b QStringList (or \ref LocalizedStringList):
  - \b alternatives. Holds alternative options for user and has type QStringList
  (or \ref LocalizedStringList).
  - \b validator. Provides validation of input text and has type QValidator or QRegExp.
 -# \b bool.
 -# \b QDate.
 -# \b QDateTime.
 -# \b int:
  - \b minValue. Holds the minimum value. By default, this property contains 0.
  - \b maxValue. Holds the maximum value. By default, this property contains 100.
 -# \b uint:
  - \b minValue. Holds the minimum value. By default, this property contains 0.
  - \b maxValue. Holds the maximum value. By default, this property contains 100.
 -# \b double:
  - \b minValue. Holds the minimum value. By default, this property contains 0.
  - \b maxValue. Holds the maximum value. By default, this property contains 100.
 -# \b QIcon:
  - \b alternatives. Holds alternative options for user and has type QList<QIcon>.
  - \b imageSize. Holds the icon size. The default property value is QSize(32,32).
 -# \b QPixmap:
  - \b alternatives. Holds alternative options for user and has type QList<QPixmap>.
  - \b imageSize. Holds the pixmap size. The default property value is QSize(32,32).
 -# \b QImage:
  - \b alternatives. Holds alternative options for user and has type QList<QImage>.
  - \b imageSize. Holds the image size. The default property value is QSize(32,32).
 -# \b QString (or \ref LocalizedString):
  - \b alternatives. Holds alternative options for user and has type QStringList
  (or \ref LocalizedStringList).
   - \b editable. Holds whether the field is editable. The default property value is false.
  - \b validator. Provides validation of input text and has type QValidator or QRegExp.
  - \b password. Holds whether the input text is displayed or every character is replaced by asterisk.
  The default property value is false. The property can be overshadowed by the property 'alternatives'.
  - \b multiline. Holds whether the text is multiline. The default property value is false.
  The property can be overshadow by the property 'alternatives'.
  - \b mandatory. Holds whether the field is mandatory. The default property value is false.
  \see AbstractDataForm::completeChanged()

 Another dynamic properties that are supported by DataForms API and do not depend on type of data():
 - \b readOnly. Holds whether the field is read only.
 By default, this property contains false. The property has highest priority and may overshadow other properties.
 - \b hideTitle. Holds whether the field title is hidden. The default property value is
 false.
 - \b titleAlternative. Holds alternative options of the title for user and has type LocalizedStringList.
 - \b hidden. Holds whether the field is hidden. The default property value is false.

 The items that \ref hasSubitems() "have subitems" may have the following properties:
 - \b columns. Holds the recommended count of the columns. By default, this property contains 1.
 Note that on some platforms the property will be ignored (for example, on mobile telephones).
 - \b verticalSpacing. Holds spacing between fields that are laid out on top of each other. If no value
 is set, the system settings will be used.
 - \b horizontalSpacing. Holds spacing between fields that are laid out side by side. If no value is set,
 the system settings will be used.

 Example of using \ref data() "data" attribute and dynamic properties:
 \code
 DataItem root("Form title");
 root << DataItem("stringList", tr("StringList"), QStringList() << "string 1" << "string 2");
 root << DataItem("bool", tr("Bool"), true);
 root << DataItem("date", tr("Date"), QDate::currentDate());
 root << DataItem("dateTime", tr("Date time"), QDateTime::currentDateTime());
 root << DataItem("int", tr("Int"), 50);
 root << DataItem("double", tr("Double"), 3.14);
 root << DataItem("icon", tr("Icon"), Icon("qutim"));
 {
  Icon qutimIcon("qutim");
  DataItem item("iconList", tr("Icon list"), qutimIcon);
  QList<QIcon> alternatives;
  alternatives << Icon("kopete") << Icon("pidgin") << qutimIcon << Icon("psi");
  item.setProperty("alternatives", qVariantFromValue(alternatives));
  root << item;
 }
 root << DataItem("image", tr("Image"), Icon(QLatin1String("qutim")).pixmap(64));
 root << DataItem("string", tr("String"), "Text field");
 {
  DataItem item("multiline", tr("Multiline"), "Multiline");
  item.setProperty("multiline", true);
  root << item;
 }
 {
  DataItem item("alternatives", tr("Alternatives"), "alternative2");
  item.setProperty("alternatives", QStringList() << "alternative1" << "alternative2");
  root << item;
 }
 {
  DataItem item("alternatives", tr("Read only ... "), "field");
  item.setReadOnly(true); // or item.setProperty("readOnly", true);
  root << item;
 }
 {
  DataItem item("alternatives", tr("You will not see the title"), "Field without title");
  item.setProperty("hideTitle", true);
  root << item;
 }
 {
  DataItem item("alternatives", tr("Title alternative 2"), "choose title");
  LocalizedStringList titleAlternatives;
  titleAlternatives
	<< tr("Title alternative 1")
	<< tr("Title alternative 2");
  item.setProperty("titleAlternatives", qVariantFromValue(titleAlternatives));
  root << item;
 }
 QWidget *widget = DataFormsBackend::instance()->get(root);
 widget->show();
 \endcode
 \image html dataforms_types_example.jpeg

 If you want to unite several fields in a group you create a parent DataItem object and
 add subitems to the object using \ref addSubitem() or \ref setSubitems().
 Example of using subitems:
 \code
 DataItem root("Form title");

 DataItem group("Group1");
 group << DataItem("field 1", tr("Field 1"), QString());
 group << DataItem("field 2", tr("Field 2"), QString());
 root << group;

 DataItem group2("Group2");
 group2 << DataItem("field 3", tr("Field 3"), QString());
 group2 << DataItem("field 4", tr("Field 4"), QString());
 root << group2;

 QWidget *widget = DataFormsBackend::instance()->get(root);
 widget->show();
 \endcode
 \image html dataforms_groups_example.jpeg

 It is possible to allow user to modify subitems using allowModifySubitems(). Here is an example:
 \code
 DataItem root("Nicks");

 DataItem defNick("Nick", QString());
 defNick.setProperty("hideTitle", true);
 root.allowModifySubitems(defNick, 10);

 DataItem nick = defNick;
 nick.setData("SokoL");
 root << nick;

 nick.setData("sokol");
 root << nick;

 QWidget *widget = DataFormsBackend::instance()->get(root);
 widget->show();
 \endcode
 \image html dataforms_modifiable_subitems_example.jpeg
 */
class LIBQUTIM_EXPORT DataItem
{
public:
	/**
	Constructs a null DataItem.
	\see isNull()
	*/
	DataItem();
	/**
	Constructs a new DataItem with the given \a name, \a title and \a data attributes.
	*/
	DataItem(const QString &name, const LocalizedString &title, const QVariant &data);
	/**
	Constructs a new DataItem with the given \a title and \a data attributes.
	*/
	DataItem(const LocalizedString &title, const QVariant &data = QVariant());
	/**
	Constructs a copy of \a other.
	This operation occurs in constant time, because DataItem is implicitly shared.
	This makes returning a DataItem from a function very fast. If a shared instance
	is modified, it will be copied (copy-on-write), and this takes linear time.
	\see operator=()
  */
	DataItem(const DataItem &other);
	/**
	Destroys the DataItem.
  */
	~DataItem();
	/**
	Assigns \a other to this data item and returns a reference to this data item.
  */
	DataItem &operator=(const DataItem &other);
	/**
	Returns the name of this data item.
	\see setName()
  */
	QString name() const;
	/**
	Sets the name of this data item to be \a itemName.
	\see name()
  */
	void setName(const QString &itemName);
	/**
	Returns the title of this data item.
	\see setTitle()
  */
	LocalizedString title() const;
	/**
	Sets the title of this data item to be \a itemTitle.
	\see title()
  */
	void setTitle(const LocalizedString &itemTitle);
	/**
	Returns the data of this data item.
  */
	QVariant data() const;
	/**
	Returns the data of this data item converted to template type T.
	If the data cannot be converted, \a def will be returned.
  */
	template<typename T>
	T data(const T &def = T()) const;
	/**
	Sets the data of this data item to be \a itemData.
  */
	void setData(const QVariant &itemData);
	/**
	Returns true if this data item is null; otherwise return false.
  */
	bool isNull() const;
	/**
	Returns the list of subitems of this data item.
	\see setSubitems(), subitems(), addSubitem(), hasSubitems().
  */
	QList<DataItem> subitems() const;
	/**
	Sets the list of subitems to be \a newSubitems.
  */
	void setSubitems(const QList<DataItem> &newSubitems);
	/**
	Returns the subitem by its \a name.
  */
	DataItem subitem(const QString &name, bool recursive = false) const;

	int removeSubitems(const QString &name, bool recursive = false);
	bool removeSubitem(const QString &name, bool recursive = false);
	DataItem takeSubitem(const QString &name, bool recursive = false);
	/**
	Adds new \a subitem to the list of subitems of this data item.
	\see operator<<()
  */
	void addSubitem(const DataItem &subitem);
	/**
	Returns true if this data item contains at least one subitem; otherwise returns false.
  */
	bool hasSubitems() const;
	/**
	Allows to modify the list of subitems by user.
	\param defaultSubitem The data item that will be used for creating a new field.
	\param maxSubitemsCount Maximum count of the subitems that user cannot exceed.
	-1 if there is no limits.
  */
	void allowModifySubitems(const DataItem &defaultSubitem, int maxSubitemsCount = -1);
	/**
	Returns true if modifying of the list of subitems by user is allowed; otherwise returns false.
	\see allowModifySubitems()
  */
	bool isAllowedModifySubitems() const;
	/**
	Returns maximum count of the subitems that user cannot exceed. Returns -1 if there is no
	limits.
	\see allowModifySubitems()
  */
	int maxSubitemsCount() const;
	/**
	Returns the data item that will be used for creating a new field.
	\see allowModifySubitems()
  */
	DataItem defaultSubitem() const;
	/**
	Returns the readOnly property.
	\see setReadOnly()
  */
	bool isReadOnly() const;
	/**
	Sets the readOnly property to be \a readOnly.
	\see isReadOnly()
  */
	void setReadOnly(bool readOnly = true);
	/**
	  Connects the data changed signal of the field to the \a method in the \a receiver.

	  The data changed signal has three paramenters:
	  - QString fieldName. The field name.
	  - QVariant data. The new data of the field.
	  - AbstractDataForm *dataForm. The pointer to the data form.
	*/
	void setDataChangedHandler(QObject *receiver, const char *method);
	/**
	  Returns the receiver of the data changed signal of the field.
	*/
	QObject *dataChangedReceiver() const;
	/**
	  Returns the method the data signal is connected to.
	*/
	const char *dataChangedMethod() const;
	/**
	Adds new \a subitem to the list of subitems of this data item.
	\see addSubitem()
  */
	DataItem &operator<<(const DataItem &subitem);
	/**
	Returns the property named \a name.
	If no such property exists, \a def will be returned.
	\see setProperty()
  */
	QVariant property(const char *name, const QVariant &def = QVariant()) const;
	/**
	Returns the \a name property converted to the template type \a T. If the value
	cannot be converted, \a def will be returned.
	\see setProperty()
  */
	template<typename T>
    T property(const char *name, const T &def = T()) const;
	/**
	Sets \a name property to be \a value.
	\see property()
  */
	void setProperty(const char *name, const QVariant &value);
	QList<QByteArray> dynamicPropertyNames() const;
protected:
#ifndef Q_QDOC
	friend class DataItemPrivate;
	QSharedDataPointer<DataItemPrivate> d;
#endif
};

template<typename T>
T DataItem::property(const char *name, const T &def) const
{
    QVariant var = property(name, QVariant::fromValue<T>(def));
    return var.value<T>();
}

class LIBQUTIM_EXPORT ReadOnlyDataItem : public DataItem
{
public:
	ReadOnlyDataItem(const LocalizedString &title, const QStringList &data);
	ReadOnlyDataItem(const LocalizedString &title, const LocalizedStringList &data);
	ReadOnlyDataItem(const LocalizedString &title, bool data);
	ReadOnlyDataItem(const LocalizedString &title, const QDate &data);
	ReadOnlyDataItem(const LocalizedString &title, const QDateTime &data);
	ReadOnlyDataItem(const LocalizedString &title, int data);
	ReadOnlyDataItem(const LocalizedString &title, uint data);
	ReadOnlyDataItem(const LocalizedString &title, double data);
	ReadOnlyDataItem(const LocalizedString &title, const QIcon &data);
	ReadOnlyDataItem(const LocalizedString &title, const QImage &data);
	ReadOnlyDataItem(const LocalizedString &title, const QPixmap &data);
	ReadOnlyDataItem(const LocalizedString &title, const QString &data);
	ReadOnlyDataItem(const LocalizedString &title, const LocalizedString &data);
};

class LIBQUTIM_EXPORT StringListDataItem : public DataItem
{
public:
	StringListDataItem(const QString &name, const LocalizedString &title,
					   const QStringList &data = QStringList(),
					   int maxStringsCount = -1);
	StringListDataItem(const QString &name, const LocalizedString &title,
					   const LocalizedStringList &data = LocalizedStringList(),
					   int maxStringsCount = -1);
};

class LIBQUTIM_EXPORT IntDataItem : public DataItem
{
public:
	IntDataItem(const QString &name, const LocalizedString &title, int data,
				int minValue = 0, int maxValue = 100);
};

class LIBQUTIM_EXPORT DoubleDataItem : public DataItem
{
public:
	DoubleDataItem(const QString &name, const LocalizedString &title, double data,
				   double minValue = 0, double maxValue = 100);
};

class LIBQUTIM_EXPORT IconDataItem : public DataItem
{
public:
	IconDataItem(const QString &name, const LocalizedString &title,
				 const QIcon &data = QIcon(),
				 const QSize &imageSize = QSize(32, 32),
				 const QList<QIcon> &alternatives = QList<QIcon>());
};

class LIBQUTIM_EXPORT ImageDataItem : public DataItem
{
public:
	ImageDataItem(const QString &name, const LocalizedString &title,
				  const QImage &data = QImage(),
				  const QSize &imageSize = QSize(32, 32),
				  const QList<QImage> &alternatives = QList<QImage>());
};

class LIBQUTIM_EXPORT PixmapDataItem : public DataItem
{
public:
	PixmapDataItem(const QString &name, const LocalizedString &title,
				   const QPixmap &data = QPixmap(),
				   const QSize &imageSize = QSize(128, 128),
				   const QList<QPixmap> &alternatives = QList<QPixmap>());
};

class LIBQUTIM_EXPORT StringChooserDataItem : public DataItem
{
public:
	StringChooserDataItem(const QString &name, const LocalizedString &title,
						  const QStringList &alternatives, const QString &data = QString(),
						  bool editable = false, QValidator *validator = 0);
	StringChooserDataItem(const QString &name, const LocalizedString &title,
						  const QStringList &alternatives, const QString &data,
						  bool editable, QRegExp validator);
	StringChooserDataItem(const QString &name, const LocalizedString &title,
						  const LocalizedStringList &alternatives,
						  const LocalizedString &data,
						  bool editable = false, QValidator *validator = 0);
	StringChooserDataItem(const QString &name, const LocalizedString &title,
						  const LocalizedStringList &alternatives,
						  const LocalizedString &data,
						  bool editable, QRegExp validator);
};

class LIBQUTIM_EXPORT MultiLineStringDataItem : public DataItem
{
public:
	MultiLineStringDataItem(const QString &name, const LocalizedString &title,
							const QString &data = QString());
	MultiLineStringDataItem(const QString &name, const LocalizedString &title,
							const LocalizedString &data);
};

class LIBQUTIM_EXPORT StringDataItem : public DataItem
{
public:
	StringDataItem(const QString &name, const LocalizedString &title,
				   const QString &data = QString(),
				   QValidator *validator = 0, bool password = false);
	StringDataItem(const QString &name, const LocalizedString &title,
				   const QString &data, QRegExp validator,
				   bool password = false);
	StringDataItem(const QString &name, const LocalizedString &title,
				   const LocalizedString &data, QValidator *validator = 0,
				   bool password = false);
	StringDataItem(const QString &name, const LocalizedString &title,
				   const LocalizedString &data, QRegExp validator,
				   bool password = false);
};

class LIBQUTIM_EXPORT AbstractDataForm : public QWidget
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
	Q_DECLARE_FLAGS(ButtonRoles, ButtonRole)
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
		RestoreDefaults    = 0x08000000
	};
	Q_DECLARE_FLAGS(StandardButtons, StandardButton)
	struct Button
	{
		LocalizedString name;
		ButtonRole role;
	};
	typedef QList<Button> Buttons;
public:
	virtual DataItem item() const = 0;
	/**
	Returns true if the state of at least one field was changed.
	Otherwise, returns false.

	\see changed()
	*/
	virtual bool isChanged() const = 0;
	/**
	Returns true if all mandatory fields are filled and every field with a validator
	is valid (i.e., current state of the validator is Acceptable). Otherwise,
	returns false.

	\see isComplete()
	*/
	virtual bool isComplete() const = 0;
	/**
	Clears the data form state.
	*/
	virtual void clearState() = 0;
	/**
	  Sets the data of the field with \a name to be the \a data.

	  Note that the type of the \a data should be exactly as
	  it was when the data form was being created.
	*/
	virtual void setData(const QString &fieldName, const QVariant &data) = 0;
	static AbstractDataForm *get(const DataItem &item,
								 StandardButtons standartButtons = NoButton,
								 const Buttons &buttons = Buttons());
public slots:
	void accept();
	void reject();
signals:
	void accepted();
	void rejected();
	void helpRequested();
	void clicked(int buttonIndex);
	void clicked(qutim_sdk_0_3::AbstractDataForm::StandardButton standartButton);
	/**
	The signal is emitted when the state of the data form has been changed.
	\see isChanged()
	*/
	void changed();
	/**
	The signal is emmitted whenever the complete state of the data form changes.

	\param complete the new complete state of the data form.
	\see isComplete()
	*/
	void completeChanged(bool complete);
protected:
	virtual void virtual_hook(int id, void *data);
};

class LIBQUTIM_EXPORT DataFormsBackend : public QObject
{
	Q_OBJECT
public:
	virtual AbstractDataForm *get(const DataItem &item,
								  AbstractDataForm::StandardButtons standartButtons = AbstractDataForm::NoButton,
								  const AbstractDataForm::Buttons &buttons = AbstractDataForm::Buttons()) = 0;
	static DataFormsBackend *instance();
};

template<typename T>
T DataItem::data(const T &def) const
{
	QVariant d = data();
	return d.canConvert<T>() ? d.value<T>() : def;
}

#ifndef Q_QDOC
Q_DECLARE_OPERATORS_FOR_FLAGS(AbstractDataForm::ButtonRoles)
Q_DECLARE_OPERATORS_FOR_FLAGS(AbstractDataForm::StandardButtons)
#endif
}

#ifndef Q_QDOC
Q_DECLARE_METATYPE(qutim_sdk_0_3::DataItem)
Q_DECLARE_METATYPE(QList<qutim_sdk_0_3::DataItem>)
#endif

#endif // DATAFORMS_H

