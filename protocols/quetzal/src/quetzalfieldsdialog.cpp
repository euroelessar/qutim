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
#include "quetzalfieldsdialog.h"
#include "quetzalaccount.h"
#include <QImageReader>
#include <QBuffer>
#include <qutim/debug.h>

QuetzalFieldsDialog::QuetzalFieldsDialog(const char *title, const char *primary,
										 const char *secondary, PurpleRequestFields *fields,
										 const char *ok_text, GCallback ok_cb,
										 const char *cancel_text, GCallback cancel_cb,
										 void *user_data, QWidget *parent)
											 : QuetzalRequestDialog(PURPLE_REQUEST_FIELDS, user_data, parent)
{
	m_ok_cb = (PurpleRequestFieldsCb) ok_cb;
	m_cancel_cb = (PurpleRequestFieldsCb) cancel_cb;
	m_fields = fields;
	DataItem root = createItem(title, primary, secondary);
	GList *group_it = purple_request_fields_get_groups(m_fields);
	bool useGroupBox = group_it->next;
	debug() << "useGroupBox" << useGroupBox;
	for (; group_it; group_it = group_it->next) {
		PurpleRequestFieldGroup *group = reinterpret_cast<PurpleRequestFieldGroup*>(group_it->data);
		DataItem groupItem;
		if (useGroupBox)
			groupItem = DataItem(purple_request_field_group_get_title(group));
		GList *field_it = purple_request_field_group_get_fields(group);
		for (; field_it; field_it = field_it->next) {
			PurpleRequestField *field = reinterpret_cast<PurpleRequestField*>(field_it->data);
			debug() << field->id << purple_request_field_is_visible(field)
					<< purple_request_field_get_type(field);
			if (!purple_request_field_is_visible(field))
				continue;
			DataItem item(purple_request_field_get_id(field),
						  purple_request_field_get_label(field),
						  QVariant());
			switch (purple_request_field_get_type(field)) {
			case PURPLE_REQUEST_FIELD_STRING:
				item.setData(QString::fromUtf8(purple_request_field_string_get_default_value(field)));
				item.setProperty("multiline", purple_request_field_string_is_multiline(field));
				item.setProperty("password", bool(purple_request_field_string_is_masked(field)));
				item.setReadOnly(!purple_request_field_string_is_editable(field));
				debug() << item.name() << item.data() << item.property("password") << item.isReadOnly();
				break;
			case PURPLE_REQUEST_FIELD_INTEGER:
				item.setData(purple_request_field_int_get_default_value(field));
				item.setProperty("minValue", G_MININT);
				item.setProperty("maxValue", G_MAXINT);
				break;
			case PURPLE_REQUEST_FIELD_BOOLEAN:
				item.setData(bool(purple_request_field_bool_get_default_value(field)));
				break;
			case PURPLE_REQUEST_FIELD_CHOICE:
				{
					GList *labels = purple_request_field_choice_get_labels(field);
					QStringList alternatives;
					for (; labels; labels = labels->next)
						alternatives << QString::fromUtf8(reinterpret_cast<char*>(labels->data));
					item.setData(alternatives.value(purple_request_field_choice_get_default_value(field)));
					item.setProperty("alternatives", alternatives);
				}
				break;
			case PURPLE_REQUEST_FIELD_LIST:
				break;
			case PURPLE_REQUEST_FIELD_LABEL:
				item.setData(purple_request_field_get_label(field));
				item.setReadOnly(true);
				item.setProperty("hideTitle", true);
				break;
			case PURPLE_REQUEST_FIELD_IMAGE:
				{
					QImageReader reader;
					QByteArray data = QByteArray::fromRawData(purple_request_field_image_get_buffer(field),
															  purple_request_field_image_get_size(field));
					QBuffer buffer(&data);
					reader.setDevice(&buffer);
					QSize imageSize = reader.size();
					if (imageSize.isValid()) {
						imageSize.setWidth(imageSize.width() * purple_request_field_image_get_scale_x(field));
						imageSize.setHeight(imageSize.height() * purple_request_field_image_get_scale_y(field));
						reader.setScaledSize(imageSize);
					}
					item.setData(qVariantFromValue(reader.read()));
					item.setProperty("imageSize", qVariantFromValue(imageSize));
					item.setReadOnly(true);
				}
				break;
			case PURPLE_REQUEST_FIELD_ACCOUNT:
				break;
			case PURPLE_REQUEST_FIELD_NONE:
			default:
				continue;
			}
			item.setProperty("mandatory", purple_request_field_is_required(field));
			if (useGroupBox)
				groupItem.addSubitem(item);
			else
				root.addSubitem(item);
		}
		if (useGroupBox)
			root.addSubitem(groupItem);
	}
	createItem(root, ok_text, cancel_text);
	return;
}

QuetzalFieldsDialog::~QuetzalFieldsDialog()
{
	purple_request_fields_destroy(m_fields);
}

void QuetzalFieldsDialog::closeRequest()
{
	onClicked(1);
}

void QuetzalFieldsDialog::onClicked(int button)
{
	debug() << Q_FUNC_INFO << button;
	PurpleRequestFieldsCb cb = button == 0 ? m_ok_cb : m_cancel_cb;
	if (button == 0) {
		const QList<DataItem> items = form()->item().subitems();
		for (int i = 0; i < items.size(); i++) {
			const DataItem &item = items.at(i);
			QByteArray id = item.name().toUtf8();
			PurpleRequestField *field = purple_request_fields_get_field(m_fields, id.constData());
			switch (purple_request_field_get_type(field)) {
			case PURPLE_REQUEST_FIELD_STRING:
				purple_request_field_string_set_value(field, item.data().toString().toUtf8().constData());
				break;
			case PURPLE_REQUEST_FIELD_INTEGER:
				purple_request_field_int_set_value(field, item.data().toInt());
				break;
			case PURPLE_REQUEST_FIELD_BOOLEAN:
				purple_request_field_bool_set_value(field, item.data().toBool());
				break;
			case PURPLE_REQUEST_FIELD_CHOICE:
				{
					QStringList alternatives = item.property("alternatives", QStringList());
					purple_request_field_choice_set_value(field, alternatives.indexOf(item.data().toString()));
				}
				break;
			case PURPLE_REQUEST_FIELD_LIST:
				break;
			case PURPLE_REQUEST_FIELD_ACCOUNT:
				break;
			case PURPLE_REQUEST_FIELD_IMAGE:
			case PURPLE_REQUEST_FIELD_LABEL:
			case PURPLE_REQUEST_FIELD_NONE:
			default:
				continue;
			}
		}
	}
	cb(userData(), m_fields);
	quetzal_request_close(PURPLE_REQUEST_FIELDS, this);
}

