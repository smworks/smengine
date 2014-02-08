/*
 * Scenario.h
 *
 *  Created on: 2014 01 28
 *      Author: Martynas Šustavičius
 */

#ifndef SCENARIO_H_
#define SCENARIO_H_

#include "Resource.h"
#include "../Vec3.h"

class Scenario : public Resource {
public:
	class Action {
	public:
		Action(float duration) : duration_(duration) {}
		virtual ~Action() {}
		virtual Action* clone() const = 0;
		/** 
		 * @param progress - animation progress from 0.0 to 1.0.
		 */
		virtual void run(float progress) = 0;
		float getDuration() { return duration_; }
	private:
		float duration_;
	};
public:
	Scenario(ServiceLocator* services);
	Scenario(const Scenario& right);
	~Scenario();

	/**
	 * @see Resource
	 */
	bool create();

	/**
	 * @see Resource
	 */
	void release();

	/**
	 * @see Resource
	 */
	SIZE getSize();

	/**
	 * @see Resource
	 */
	Resource::Type getType();

	/**
	 * @see Resource
	 */
	bool isValid();

	/**
	 * Moves specified node from start to end position over
	 * specified period of time in milliseconds.
	 * @param node - node that will be moved.
	 * @param start - starting position.
	 * @param end - ending position
	 * @param time - movement duration in milliseconds.
	 */
	void move(Node* node, Vec3 start, Vec3 end, float time);

	/**
	 * Halts scenario execution for specified period of
	 * time in milliseconds.
	 * @param time - wait duration in milliseconds.
	 */
	void wait(float time);

	/**
	 * @return Reference to action array.
	 */
	vector<Action*>& getActions();

	/** 
	 * @param progress - time elapsed since the
	 * start of scenario execution.
	 */
	void setProgress(float progress);

	/**
	 * @return Time elapsed since the start of scenario execution.
	 */
	float getProgress();

	/**
	 * @param index - current action index.
	 */
	void setIndex(SIZE index);

	/**
	 * @return Current action index.
	 */
	SIZE getIndex();
private:
	vector<Action*> actions_;
	float progress_;
	SIZE index_;
};

#endif