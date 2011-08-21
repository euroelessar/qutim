import QtQuick 1.0
import com.nokia.meego 1.0
import org.qutim 0.3

PageStackWindow {
	id: root
	ServiceManager {
		id: serviceManager
		onValueChanged: {
			if (key == "contactList")
				root.initialPage = contactList.create(root)
		}
//		onContactListChanged: root.initialPage = contactList.create(root)
	}
}
