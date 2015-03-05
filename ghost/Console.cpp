/*
 * Console.cpp
 *
 *  Created on: 2012.10.15
 *      Author: MS
 */

#include "Console.h"
#include "Multiplatform/ServiceLocator.h"
#include "Resources/GUIImage.h"
#include "Resources/GUIText.h"
#include "Node.h"
#include "ResourceManager.h"
#include "ThreadManager.h"
#include "Thread.h"
#include <iostream>
#include "ScriptManager.h"
#include "Multiplatform/Socket.h"
#include "Network/HttpRequest.h"
#include "Network/HttpResponse.h"

class CMDTask : public Task {
public:
	CMDTask(ServiceLocator* services) : services(services), show(false), isShowing(false) {}
	~CMDTask() {}
	void run() {
		AllocConsole();
		freopen("CONIN$", "r", stdin);
		freopen("CONOUT$", "w", stdout);
		ShowWindow(GetConsoleWindow(), SW_HIDE);
		string line;
		while (true) {
			if (!isShowing && show || isShowing && !show) {
				ShowWindow(GetConsoleWindow(), show ? SW_SHOW : SW_HIDE);
				isShowing = show;
			}
			if (show) {
				cout << "Enter command: ";
				getline(cin, line);
				if (line == "close") {
					show = false;
					continue;
				} else if (line == "test") {
					Socket* s = services->createSocket(Socket::Type::UDP);
					HttpRequest r("127.0.0.1");
					r.setPort(8888);
					string entity = "Hello socket. It's ghost :)";
					INT8* bytes = NEW INT8[entity.size()];
					memcpy(bytes, entity.c_str(), entity.size());
					r.setEntity(bytes, entity.size());
					s->send(&r);
					HttpResponse* rs = s->receive();
					if (rs != 0) {
						LOGD("TEST finished. Result: %s", rs->getContent());
						delete rs;
					}
					delete s;
				} else {
					string res = services->getScriptManager()->executeCode(line);
					if (res.length() > 0) {
						cout << res << endl;
					}
				}
			}
			if (services->isFinished()) {
				FreeConsole();
				return;
			}
			this_thread::sleep_for(chrono::milliseconds(100));
		}
	}
	volatile bool show;
private:
	bool isShowing;
	ServiceLocator* services;
};

Console::Console(ServiceLocator* services) : services(services), cmdTask(0) {}

Console::~Console() {
}

bool Console::isShowing() {
	return cmdTask == 0 ? false : cmdTask->show;
}

void Console::show() {
	if (cmdTask == 0)
		services->getThreadManager()->execute(cmdTask = NEW CMDTask(services));
	cmdTask->show = true;
}