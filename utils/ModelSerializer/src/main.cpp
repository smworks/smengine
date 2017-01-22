#include "../../../ghost/Multiplatform/Ghost.h"
#include "../../../ghost/Multiplatform/Win32/WindowsServiceLocator.h"
#include "../../../ghost/ResourceManager.h"
#include "../../../ghost/ThreadManager.h"

#include "ObjParser.h"
#include <iostream>

ServiceLocator* getServiceLocator()
{
	auto* wsl = new WindowsServiceLocator();
	wsl->setScreenWidth(800);
	wsl->setScreenHeight(600);
	wsl->provide(NEW ResourceManager(wsl));
	wsl->provide(NEW ThreadManager(wsl));
	PROFILE("Finished creating engine object.");
	return wsl;
}

bool areColorsEqual(Color left, Color right)
{
	return left.getA() == right.getA() && left.getR() == right.getR()
		&& left.getG() == right.getG() && left.getB() == right.getB();
}

int main()
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
#endif

	auto* sl = getServiceLocator();
	try {
		cout << "Enter model file to convert: ";
		string file;
		getline(cin, file);
		cout << "Loading file: " << file.c_str() << endl;
		ModelData md = ObjParser(sl).parse(file);
		md.serializeToFile(file + ".sm");
		cout << "Successfully serialized model" << endl;
	}
	catch (exception e) {
		cout << "Exception occured: " << e.what() << endl;
	}
	sl->release();
	delete sl;
	return 0;
}
