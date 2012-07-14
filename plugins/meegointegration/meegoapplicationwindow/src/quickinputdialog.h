#ifndef MEEGOINTEGRATION_QUICKINPUTDIALOG_H
#define MEEGOINTEGRATION_QUICKINPUTDIALOG_H

#include <QInputDialog>

namespace MeegoIntegration {

class QuickInputDialog : public QObject
{
    Q_OBJECT
    Q_ENUMS(InputMode)
    Q_FLAGS(InputDialogOption InputDialogOptions)
    Q_PROPERTY(QString windowTitle READ windowTitle CONSTANT)
    Q_PROPERTY(InputMode inputMode READ inputMode CONSTANT)
    Q_PROPERTY(QString labelText READ labelText CONSTANT)
    Q_PROPERTY(InputDialogOptions options READ options CONSTANT)
    Q_PROPERTY(QString textValue READ textValue WRITE setTextValue NOTIFY textValueChanged)
    Q_PROPERTY(int intValue READ intValue WRITE setIntValue NOTIFY intValueChanged)
    Q_PROPERTY(int doubleValue READ doubleValue WRITE setDoubleValue NOTIFY doubleValueChanged)
    Q_PROPERTY(QLineEdit::EchoMode textEchoMode READ textEchoMode CONSTANT)
    Q_PROPERTY(bool comboBoxEditable READ isComboBoxEditable CONSTANT)
    Q_PROPERTY(QStringList comboBoxItems READ comboBoxItems CONSTANT)
    Q_PROPERTY(int intMinimum READ intMinimum CONSTANT)
    Q_PROPERTY(int intMaximum READ intMaximum CONSTANT)
    Q_PROPERTY(int intStep READ intStep CONSTANT)
    Q_PROPERTY(double doubleMinimum READ doubleMinimum CONSTANT)
    Q_PROPERTY(double doubleMaximum READ doubleMaximum CONSTANT)
    Q_PROPERTY(int doubleDecimals READ doubleDecimals CONSTANT)
    Q_PROPERTY(QString okButtonText READ okButtonText CONSTANT)
    Q_PROPERTY(QString cancelButtonText READ cancelButtonText CONSTANT)
public:
    explicit QuickInputDialog(QInputDialog *parent);
    enum InputDialogOption {
        NoButtons                   = QInputDialog::NoButtons,
        UseListViewForComboBoxItems = QInputDialog::UseListViewForComboBoxItems
    };

    Q_DECLARE_FLAGS(InputDialogOptions, InputDialogOption)

    enum InputMode {
        TextInput   = QInputDialog::TextInput,
        IntInput    = QInputDialog::IntInput,
        DoubleInput = QInputDialog::DoubleInput
    };
    
    QString windowTitle() const;
    QuickInputDialog::InputMode inputMode() const;
    QString labelText() const;
    QuickInputDialog::InputDialogOptions options() const;
    
    void setTextValue(const QString &textValue);
    QString textValue() const;
    void setIntValue(int intValue);
    int intValue() const;
    void setDoubleValue(double doubleValue);
    double doubleValue() const;
    
    QLineEdit::EchoMode textEchoMode() const;
    bool isComboBoxEditable() const;
    QStringList comboBoxItems() const;
    int intMinimum() const;
    int intMaximum() const;
    int intStep() const;
    double doubleMinimum() const;
    double doubleMaximum() const;
    int doubleDecimals() const;
    QString okButtonText() const;
    QString cancelButtonText() const;
    
public slots:
    void accept();
    void reject();
    
signals:
    void textValueChanged(const QString &textValue);
    void intValueChanged(int textValue);
    void doubleValueChanged(double textValue);
    
public slots:
    
private:
    QInputDialog *m_dialog;
};

} // namespace MeegoIntegration

#endif // MEEGOINTEGRATION_QUICKINPUTDIALOG_H
