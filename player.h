#pragma once

#ifndef PLAYER_H
#define PLAYER_H

class player {
public:
	void begin();
	void mainloop();
private:
	void movement();
	void cameraMovement();
};

#endif