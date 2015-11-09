#include <iostream>
#include "Sampler.h"
#include "Renderer.h"

using namespace std;

int main()
{
	cout << "BAND_NUM: " << BAND_NUM << endl;
	cout << "LOBE_NUM: " << LOBE_NUM << endl;
	cout << "DIR_NUM: " << DIR_NUM << endl;
	cout << "SAMPLE_NUM: " << SQRT_SAMPLES_NUM * SQRT_SAMPLES_NUM << endl;


	//sampling
	Sampler sampler;
	sampler.generateSamples();

	//create a set of direction
	Directions directions;
	directions.generateDir(DIR_NUM);


	if (FIT_SHOW != 0)
	{
		sampler.generateCoeffsF();
		sampler.generateLobeF(directions);
		sampler.precomputeColorF();
	}
	

	Renderer renderer(600, 600);
	renderer.compileShaderFromFile("./shaders/verShader.glsl", "./shaders/fragShader.glsl");
	renderer.setUniform();
	renderer.renderSamples(sampler);
}