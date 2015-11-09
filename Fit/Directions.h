#ifndef __DIRECTION_H__
#define __DIRECTION_H__

#include "Global.h"
#include "SHEval.h"

class DIR
{
public:
	glm::vec3 xyz;
	LFLOAT theta;
	LFLOAT phi;

	LFLOAT shValues[BAND_NUM * BAND_NUM];

	DIR()
	{
	}

	void init(const LFLOAT x, const LFLOAT y, const LFLOAT z, const LFLOAT theta, const LFLOAT phi);

private:
	void calculateShValues();
};

class Directions
{
public:
	std::vector<DIR> directions;

	Directions()
	{
	}

	void generateDir(const int dirNum);
};

#endif