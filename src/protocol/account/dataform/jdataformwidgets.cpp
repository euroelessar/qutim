#include "jdataformwidgets.h"

namespace Jabber
{
	JDFBoolean::JDFBoolean(QWidget *parent) : QCheckBox(parent)
	{
	}

	JDFBoolean::~JDFBoolean()
	{
	}

	JDFHidden::JDFHidden(QWidget *parent) : QObject(parent)
	{
	}

	JDFHidden::~JDFHidden()
	{
	}

	JDFMultiJID::JDFMultiJID(QWidget *parent) : QListWidget(parent)
	{
	}

	JDFMultiJID::~JDFMultiJID()
	{
	}

	JDFSingleJID::JDFSingleJID(QWidget *parent)
	{
	}

	JDFSingleJID::~JDFSingleJID()
	{
	}

	JDFMultiList::JDFMultiList(QWidget *parent) : QListWidget(parent)
	{
	}

	JDFMultiList::~JDFMultiList()
	{
	}

	JDFSingleList::JDFSingleList(QWidget *parent) : QComboBox(parent)
	{
	}

	JDFSingleList::~JDFSingleList()
	{
	}

	JDFMultiText::JDFMultiText(QWidget *parent) : QTextEdit(parent)
	{
	}

	JDFMultiText::~JDFMultiText()
	{
	}

	JDFSingleText::JDFSingleText(QWidget *parent) : QLineEdit(parent)
	{
	}

	JDFSingleText::~JDFSingleText()
	{
	}

}
