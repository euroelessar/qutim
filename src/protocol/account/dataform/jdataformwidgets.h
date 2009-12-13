#ifndef JDATAFORMWIDGETS_H
#define JDATAFORMWIDGETS_H

#include <QCheckBox>
#include <QComboBox>
#include <QTextEdit>
#include <QListWidget>
#include <QLabel>
#include <QLineEdit>
#include "jdataformelement.h"

namespace Jabber
{
	class JDFBoolean : public QCheckBox, public JDataFormElement
	{
		Q_OBJECT
		Q_INTERFACES(Jabber::JDataFormElement)

		public:
			JDFBoolean(QWidget *parent = 0);
			~JDFBoolean();
			QString dfValue() {return isChecked() ? "1" : "0";}
			void dfSetValue(const QString &value) {setChecked(value == "1" ? true : false);}
			QString dfName() {return objectName();}
			void dfSetName(const QString &name) {setObjectName(name);}
			QObject *instance() {return this;}
			/*The field enables an entity to gather or provide an either-or choice between two options. The default value is "false". [10]*/
	};

	class JDFHidden : public QObject, public JDataFormElement
	{
		Q_OBJECT
		Q_INTERFACES(Jabber::JDataFormElement)

		public:
			JDFHidden(QWidget *parent = 0);
			~JDFHidden();
			QString dfValue() {return m_text;}
			void dfSetValue(const QString &value) {m_text = value;}
			QString dfName() {return objectName();}
			void dfSetName(const QString &name) {setObjectName(name);}
			QObject *instance() {return this;}
		private:
			QString m_text;
		/*The field is not shown to the form-submitting entity, but instead is returned with the form. The form-submitting entity SHOULD NOT modify the value of a hidden field, but MAY do so if such behavior is defined for the "using protocol".*/
	};

	class JDFMultiJID : public QListWidget, public JDataFormElement
	{
		Q_OBJECT
		Q_INTERFACES(Jabber::JDataFormElement )

		public:
			JDFMultiJID(QWidget *parent = 0);
			~JDFMultiJID();
			QString dfValue() {}
			void dfSetValue(const QString &value) {}
			QString dfName() {return objectName();}
			void dfSetName(const QString &name) {setObjectName(name);}
			QObject *instance() {return this;}
		/*id-multi	The field enables an entity to gather or provide multiple Jabber IDs. Each provided JID SHOULD be unique (as determined by comparison that includes application of the Nodeprep, Nameprep, and Resourceprep profiles of Stringprep as specified in XMPP Core), and duplicate JIDs MUST be ignored. */
	};

	class JDFSingleJID : public QLineEdit, public JDataFormElement
	{
		Q_OBJECT
		Q_INTERFACES(Jabber::JDataFormElement )

		public:
			JDFSingleJID(QWidget *parent = 0);
			~JDFSingleJID();
			QString dfValue() {return text();}
			void dfSetValue(const QString &value) {setText(value);}
			QString dfName() {return objectName();}
			void dfSetName(const QString &name) {setObjectName(name);}
			QObject *instance() {return this;}
		/*jid-single	The field enables an entity to gather or provide a single Jabber ID. */
	};

	class JDFMultiList : public QListWidget, public JDataFormElement
	{
		Q_OBJECT
		Q_INTERFACES(Jabber::JDataFormElement )

		public:
			JDFMultiList(QWidget *parent = 0);
			~JDFMultiList();
			QString dfValue() {}
			void dfSetValue(const QString &value) {}
			QString dfName() {return objectName();}
			void dfSetName(const QString &name) {setObjectName(name);}
			QObject *instance() {return this;}
		/*list-multi	The field enables an entity to gather or provide one or more options from among many. A form-submitting entity chooses one or more items from among the options presented by the form-processing entity and MUST NOT insert new options. The form-submitting entity MUST NOT modify the order of items as received from the form-processing entity, since the order of items MAY be significant.**/
	};

	class JDFSingleList : public QComboBox, public JDataFormElement
	{
		Q_OBJECT
		Q_INTERFACES(Jabber::JDataFormElement )

		public:
			JDFSingleList(QWidget *parent = 0);
			~JDFSingleList();
			QString dfValue() {return currentText();}
			void dfSetValue(const QString &value) {setCurrentIndex(findText(value));}
			QString dfName() {return objectName();}
			void dfSetName(const QString &name) {setObjectName(name);}
			QObject *instance() {return this;}
		/*list-single	The field enables an entity to gather or provide one option from among many. A form-submitting entity chooses one item from among the options presented by the form-processing entity and MUST NOT insert new options. **/
	};

	class JDFMultiText : public QTextEdit, public JDataFormElement
	{
		Q_OBJECT
		Q_INTERFACES(Jabber::JDataFormElement )

		public:
			JDFMultiText(QWidget *parent = 0);
			~JDFMultiText();
			QString dfValue() {return toPlainText();}
			void dfSetValue(const QString &value) {setPlainText(value);}
			QString dfName() {return objectName();}
			void dfSetName(const QString &name) {setObjectName(name);}
			QObject *instance() {return this;}
		/*text-multi	The field enables an entity to gather or provide multiple lines of text. ***/
	};

	class JDFSingleText : public QLineEdit, public JDataFormElement
	{
		Q_OBJECT
		Q_INTERFACES(Jabber::JDataFormElement )

		public:
			JDFSingleText(QWidget *parent = 0);
			~JDFSingleText();
			QString dfValue() {return text();}
			void dfSetValue(const QString &value) {setText(value);}
			QString dfName() {return objectName();}
			void dfSetName(const QString &name) {setObjectName(name);}
			QObject *instance() {return this;}
		/*text-single The field enables an entity to gather or provide a single line or word of text, which may be shown in an interface. This field type is the default and MUST be assumed if a form-submitting entity receives a field type it does not understand.*/
	};
}

#endif // JDATAFORMWIDGETS_H
