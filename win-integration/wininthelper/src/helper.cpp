#include <winsock2.h>
#include <string>

typedef LONG LSTATUS; //fucking winAPI

using namespace std;

const int connectPort = 35134;

int TransmitterMain(string &cmd)
{
	int port = connectPort;
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET sk;
	sk = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sk == INVALID_SOCKET) {
		WSACleanup();
		return 1;
	}

	sockaddr_in sai;
	sai.sin_family      = AF_INET;
	sai.sin_addr.s_addr = inet_addr("127.0.0.1");
	sai.sin_port        = htons(port);

	if (connect(sk, reinterpret_cast<SOCKADDR*>(&sai), sizeof(sai)) == SOCKET_ERROR) {
		WSACleanup();
		return 2;
	}

	send(sk, cmd.c_str(), cmd.size(), 0);
	closesocket(sk);
	WSACleanup();
	return 0;
}

int AssociaterMain()
{
	HKEY xmppKey, iconKey, cmdKey, appKey;
	LSTATUS st = RegCreateKeyExA(HKEY_CLASSES_ROOT, "xmpp", 0, 0, 0, KEY_WRITE, 0, &xmppKey, 0);
	if (st == ERROR_SUCCESS) {
		const char *xmppDefValue = "URL:Jabber ID (XMPP)";
		wchar_t thisPath[MAX_PATH];

		GetModuleFileNameW(0, thisPath, MAX_PATH);
		wstring icon = thisPath;
		wstring command = thisPath;
		icon     = icon.substr(0, icon.find_last_of('\\')+1)+L"qutim.exe,1";
		command += L" transmitter xmpp-proto %1";

		RegCreateKeyExA(xmppKey, "DefaultIcon", 0, 0, 0, KEY_WRITE, 0, &iconKey, 0);
		RegCreateKeyExA(xmppKey, "shell\\open\\command", 0, 0, 0, KEY_WRITE, 0, &cmdKey, 0);
		RegSetValueExA(xmppKey, 0, 0, REG_SZ, reinterpret_cast<const BYTE*>(xmppDefValue), strlen(xmppDefValue)+1);
		RegSetValueExA(xmppKey, "URL Protocol", 0, REG_SZ, reinterpret_cast<const BYTE*>(""), 1);
		RegSetValueExW(iconKey, 0, 0, REG_SZ, reinterpret_cast<const BYTE*>(icon.c_str()),    (icon.size()+1)   *sizeof(wchar_t));
		RegSetValueExW(cmdKey,  0, 0, REG_SZ, reinterpret_cast<const BYTE*>(command.c_str()), (command.size()+1)*sizeof(wchar_t));
		RegCloseKey(cmdKey);
		RegCloseKey(iconKey);

		const char *friendlyNameVal = "Applications\\wininthelper.exe";
		const char *firendlyName    = "QutIM";
		RegCreateKeyExA(HKEY_CLASSES_ROOT, friendlyNameVal, 0, 0, 0, KEY_WRITE, 0, &appKey, 0);
		RegSetValueExA(appKey, "FriendlyAppName", 0, REG_SZ, reinterpret_cast<const BYTE*>(firendlyName), strlen(firendlyName)+1);
		RegCloseKey(appKey);
		RegCloseKey(xmppKey);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR cmdstr, int)
{
	string cmd(cmdstr);
	size_t space = cmd.find(' ');
	string second, first  = cmd.substr(0, space);
	if (space != string::npos)
		second = cmd.substr(first.size()+1);
	if (first == "transmitter")
		return TransmitterMain(second);
	else if (first == "assocreg")
		return AssociaterMain();
	else
		return 0;
}
