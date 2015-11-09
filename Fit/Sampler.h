/***************************************
*author: guohongzhi zju
*date:2015.8.22
*func: 在2D空间采样，然后将其映射到球面上
*然后计算每个样本点的SH
****************************************/
/***************************************
*author: guohongzhi zju
*date:2015.8.22
*func: 在2D空间采样，然后将其映射到球面上
*然后计算每个样本点的SH
****************************************/
#ifndef _SAMPLER_H_
#define _SAMPLER_H_

#include "Global.h"
#include "Directions.h"


class SAMPLE
{
public:
	glm::vec3 xyz;
	LFLOAT theta;
	LFLOAT phi;
	LFLOAT shValues[BAND_NUM * BAND_NUM];
	LFLOAT colorF;
	LFLOAT approximateColorF;
	glm::vec3 colorRGB;
	glm::vec3 approximateColorRGB;

	SAMPLE(const SAMPLE& sample);
	SAMPLE(){}
	~SAMPLE()
	{
	}
};

class Sampler
{
public:
	std::vector<SAMPLE> samples;
	LFLOAT coeffsF[BAND_NUM * BAND_NUM];
	LFLOAT lobesF[LOBE_NUM][BAND_NUM * BAND_NUM + 3];


	//get the size of samples
	unsigned size();
	//generate samples
	void generateSamples();
	void generateCoeffsF();
	void precomputeColorF();
	void generateLobeF(Directions &dirs);

	SAMPLE &operator[] (unsigned int index);
};

#endif