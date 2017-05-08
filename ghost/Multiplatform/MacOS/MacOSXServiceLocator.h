#ifndef MACOSXSERVICELOCATOR_H_
#define MACOSXSERVICELOCATOR_H_

#include "../Ghost.h"
#include "../ServiceLocator.h"

class MacOSXServiceLocator : public ServiceLocator {
public:
	MacOSXServiceLocator();
	~MacOSXServiceLocator();
    double getFrameTime();
    double updateTimer(float sleep = 0.0f);
    Socket* createSocket(SocketParams params);
    GraphicsManager* getGraphicsManager();
    FileManager* getFileManager();
    SoundManager* getSoundManager();
    Database* getDB();
private:
	int nScreenWidth_;
	int nScreenHeight_;
	SIZE timeBefore_, timeAfter_, timeElapsed_;
	double frameDuration_;
	GraphicsManager* graphicsManager_;
	FileManager* fileManager_;
    SoundManager* soundManager_;
    Database* database_;
};

#endif
