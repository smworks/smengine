#ifndef ENGINE_H_
#define ENGINE_H_

#include "Multiplatform/Ghost.h"
#include "ServiceProvider.h"
class Node;
class ServiceLocator;
class GUIText;
class Console;
class Task;

class Engine : public ServiceProvider {
public:
	explicit Engine(ServiceLocator* services);
	~Engine();

	void loadScene(string script);
	void computeFrame();
	void resume();
	void pause();

	/**
	 * Update node logic down the hierarchy.
	 * recalculate all node matrices that belong to this node.
	 * @param node - node that will be updated.
	 */
	void updateNodes(Node* node, bool force = false) const;
	void resizeScreen(UINT32 width, UINT32 height);

	/**
	 * Informs all resources that screen size has changed.
	 * @param node - pointer to parent scene node.
	 */
	static void resizeResources(Node* node);

	static mutex& getMutex();
	static void executeOnMainThread(Task* task);
private:
	static void executeTasks();

	double time;
	bool running;
	Console* console;
	static queue<Task*>* tasks;
	static mutex* mut;
};

#endif /* ENGINE_H_ */
