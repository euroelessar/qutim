#include "quickinputdialog.h"

namespace MeegoIntegration {

QuickInputDialog::QuickInputDialog(QInputDialog *parent) :
    QObject(parent), m_dialog(parent)
{
}

QString QuickInputDialog::windowTitle() const
{
    return m_dialog->windowTitle();
}

QuickInputDialog::InputMode QuickInputDialog::inputMode() const
{
    return static_cast<InputMode>(m_dialog->inputMode());
}

QString QuickInputDialog::labelText() const
{
    return m_dialog->labelText();
}

QuickInputDialog::InputDialogOptions QuickInputDialog::options() const
{
    return static_cast<InputDialogOption>(int(m_dialog->options()));
}

void QuickInputDialog::setTextValue(const QString &textValue)
{
    if (m_dialog->textValue() != textValue) {
        m_dialog->setTextValue(textValue);
        emit textValueChanged(textValue);
    }
}

QString QuickInputDialog::textValue() const
{
    return m_dialog->textValue();
}

void QuickInputDialog::setIntValue(int intValue)
{
    if (m_dialog->intValue() != intValue) {
        m_dialog->setIntValue(intValue);
        emit intValueChanged(intValue);
    }
}

int QuickInputDialog::intValue() const
{
    return m_dialog->intValue();
}

void QuickInputDialog::setDoubleValue(double doubleValue)
{
    if (m_dialog->doubleValue() != doubleValue) {
        m_dialog->setDoubleValue(doubleValue);
        emit doubleValueChanged(doubleValue);
    }
}

double QuickInputDialog::doubleValue() const
{
    return m_dialog->doubleValue();
}

QLineEdit::EchoMode QuickInputDialog::textEchoMode() const
{
    return m_dialog->textEchoMode();
}

bool QuickInputDialog::isComboBoxEditable() const
{
    return m_dialog->isComboBoxEditable();
}

QStringList QuickInputDialog::comboBoxItems() const
{
    return m_dialog->comboBoxItems();
}

int QuickInputDialog::intMinimum() const
{
    return m_dialog->intMinimum();
}

int QuickInputDialog::intMaximum() const
{
    return m_dialog->intMaximum();
}

int QuickInputDialog::intStep() const
{
    return m_dialog->intStep();
}

double QuickInputDialog::doubleMinimum() const
{
    return m_dialog->doubleMinimum();
}

double QuickInputDialog::doubleMaximum() const
{
    return m_dialog->doubleMaximum();
}

int QuickInputDialog::doubleDecimals() const
{
    return m_dialog->doubleDecimals();
}

QString QuickInputDialog::okButtonText() const
{
    return m_dialog->okButtonText();
}

QString QuickInputDialog::cancelButtonText() const
{
    return m_dialog->cancelButtonText();
}

void QuickInputDialog::accept()
{
    m_dialog->accept();
}

void QuickInputDialog::reject()
{
    m_dialog->reject();
}

}
 // namespace MeegoIntegration
