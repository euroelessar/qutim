// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import com.nokia.meego 1.0
import org.qutim 0.3

 QueryDialog {
     id: dialog
     property QtObject widget: null
     property bool hasWidget: widget !== null
     titleText: hasWidget ? widget.windowTitle : ""
     message: hasWidget ? widget.text : ""
     acceptButtonText: fixName(guessAcceptButtonName(widget.standardButtons).text)
     rejectButtonText: fixName(guessRejectButtonName(widget.standardButtons).text)
     
     property variant positiveButtons: [
         { value: QMessageBox.Ok, text: "OK" },
         { value: QMessageBox.Save, text: "Save" },
         { value: QMessageBox.SaveAll, text: "Save all" },
         { value: QMessageBox.Open, text: "Open" },
         { value: QMessageBox.Yes, text: "&Yes" },
         { value: QMessageBox.YesToAll, text: "Yes to &All" },
         { value: QMessageBox.Abort, text: "Abort" },
         { value: QMessageBox.Retry, text: "Retry" },
         { value: QMessageBox.Apply, text: "Apply" }
     ]
     property variant negativeButtons: [
         { value: QMessageBox.Cancel, text: "Cancel" },
         { value: QMessageBox.No, text: "&No" },
         { value: QMessageBox.NoToAll, text: "N&o to All" },
         { value: QMessageBox.Ignore, text: "Ignore" },
         { value: QMessageBox.Close, text: "Close" },
         { value: QMessageBox.Discard, text: "Discard" }
     ]
     
     function fixName(text) {
         text = qsTranslate("QDialogButtonBox", text);
         return text.replace("&", "");
     }
     
     function guessAcceptButtonName(flags) {
         for (var i = 0; i < positiveButtons.length; ++i) {
             if (flags & positiveButtons[i].value)
                 return positiveButtons[i];
         }
         return positiveButtons[0];
     }
     function guessRejectButtonName(flags) {
         for (var i = 0; i < negativeButtons.length; ++i) {
             if (flags & negativeButtons[i].value)
                 return negativeButtons[i];
         }
         return { value: QMessageBox.Cancel, name: "" };
     }
     
     onAccepted: {
         var value = guessAcceptButtonName(widget.standardButtons).value;
         widget.done(value);
     }
     onRejected: {
         var value = guessRejectButtonName(widget.standardButtons).value;
         widget.done(value);
     }
}
