/*
 * GameEngine.h
 *
 * Copyright (c) Thomas Basler, 2018
 *
 * Authors:
 *   Thomas Basler <thomas@familie-basler.net>
 *
 * This work is licensed under the terms of the GNU GPL, version 3.  See
 * the LICENSE file in the top-level directory.
 */

#include "GameEngine.h"
#include "ModeHandler.h"

GameEngine::GameEngine() :
	initState(&startup)
{}

void GameEngine::begin()
{
	evQueueSize = EVENT_QUEUE_SIZE;
	evQueueFlush();

	currentState = initState;
	currentState->entryAction(this, NoParam);
}

bool GameEngine::update()
{
	if (evQueueIsEmpty()) {
		return false;
	}
	Event e = evQueue[evQueueHead];
  	evQueueHead = (evQueueHead + 1) & (evQueueSize - 1);

	for (uint8_t i = 0; i < sizeof(fsmApp) / sizeof(Transition); i++) {
		if (!(
				(currentState == fsmApp[i].currentState) &&
				(fsmApp[i].event == NoEvent || e == fsmApp[i].event) &&
				(fsmApp[i].gameType == Independent || fsmApp[i].gameType == Mode.getGameType())
			)) {
			continue;
		}

		if (e == Tick) {
			if (fsmApp[i].maxTimeBeforeTransition != 0 && timeInState > fsmApp[i].maxTimeBeforeTransition) {
				fsmApp[i].currentState->exitAction(this);
				currentState = fsmApp[i].nextState;
				evQueueFlush();
				currentState->entryAction(this, fsmApp[i].parameter);
				timeInState = 0;
			}
			else {
				// System Tick
				fsmApp[i].currentState->tickAction(this);
			}
		}
		else {
			timeInState = 0;
			fsmApp[i].currentState->exitAction(this);
			currentState = fsmApp[i].nextState;
			evQueueFlush();
			currentState->entryAction(this, fsmApp[i].parameter);
		}

		break;
	}

	return true;
}

void GameEngine::sendEvent(Event e)
{
	if (!evQueueIsFull()) {
		evQueue[evQueueTail] = e;
		evQueueTail = (evQueueTail + 1) & (evQueueSize - 1);
	}
}