#include "Game.h"


void Game::Ready() {
	addChildObject(new Player1Paddle);
	addChildObject(new Player2Paddle);
	addChildObject(new Ball);
}