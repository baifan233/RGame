#pragma once

#include "Vector2.h"
#include"Polygon.h"
#include"FrameBufferObject.h"
#include <math.h>
#include"БъЭЗ.h"


//The number of points to draw a circle
#define NUM_POINTS 30

class Scene;

class Light {
public:
	Light(vector2f, float, float);
	Light(vector2f, float, float, float, float, float);

	void tick();

	void draw(std::vector<m_Polygon*>*);
	void drawAlpha(std::vector<m_Polygon*>*);
	void drawShadows(std::vector<m_Polygon*>*);

	vector2f pos;
	float radius;
	float intensity;

	float red, green, blue;

	bool castShadows;

private:
	void init();
};