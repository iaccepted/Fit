#include "Sampler.h"
#include "SHEval.h"
#include "Global.h"
#include "BFGS.h"

SAMPLE::SAMPLE(const SAMPLE &sample)
{
	int nSamples = SQRT_SAMPLES_NUM * SQRT_SAMPLES_NUM;
	this->xyz = sample.xyz;
	this->theta = sample.theta;
	this->phi = sample.phi;
	memcpy(this->shValues, sample.shValues, nSamples * sizeof(LFLOAT));
	this->colorF = sample.colorF;
	this->approximateColorF = sample.approximateColorF;
	this->colorRGB = sample.colorRGB;
	this->approximateColorRGB = sample.approximateColorRGB;
}

void Sampler::generateSamples()
{
	int numSamples = SQRT_SAMPLES_NUM * SQRT_SAMPLES_NUM;
	int numFunctions = BAND_NUM * BAND_NUM;
	SHEvalFunc SHEval[] = { SHEval3, SHEval3, SHEval3, SHEval3, SHEval4, SHEval5, SHEval6, SHEval7, SHEval8, SHEval9, SHEval10 };

	samples.resize(numSamples);

	//srand(unsigned(time(NULL)));

	LFLOAT minx = (LFLOAT)INT_MAX, maxx = (LFLOAT)INT_MIN;

	unsigned index = 0;
	for (int i = 0; i < SQRT_SAMPLES_NUM; ++i)
	{
		for (int j = 0; j < SQRT_SAMPLES_NUM; ++j)
		{
			LFLOAT a = (i + ((LFLOAT)rand() / RAND_MAX)) / (LFLOAT)SQRT_SAMPLES_NUM;
			LFLOAT b = (j + ((LFLOAT)rand() / RAND_MAX)) / (LFLOAT)SQRT_SAMPLES_NUM;

			LFLOAT theta = 2 * acos(sqrt(1 - a));
			LFLOAT phi = 2 * M_PI * b;
			LFLOAT x = sin(theta) * cos(phi);
			LFLOAT y = sin(theta) * sin(phi);
			LFLOAT z = cos(theta);
			samples[index].xyz = glm::vec3(x, y, z);
			samples[index].phi = phi;
			samples[index].theta = theta;


			SHEval[BAND_NUM](x, y, z, samples[index].shValues);

			samples[index].colorF = std::max(0.0, 5.0 * cos(theta) - 4.0) + \
				std::max(0.0, -4.0 * sin(theta - M_PI) * cos(phi - 2.5) - 3.0);

			++index;
		}

	}
}



void Sampler::generateCoeffsF()
{
	unsigned nFuncs = BAND_NUM * BAND_NUM;

	for (unsigned l = 0; l < nFuncs; ++l)
	{
		this->coeffsF[l] = 0;

		unsigned nSamples = SQRT_SAMPLES_NUM * SQRT_SAMPLES_NUM;
		for (unsigned sampleIdx = 0; sampleIdx < nSamples; ++sampleIdx)
		{
			SAMPLE &curSample = samples[sampleIdx];

			this->coeffsF[l] += curSample.colorF * curSample.shValues[l];
		}

		this->coeffsF[l] *= 4.0 * M_PI / (LFLOAT)nSamples;
	}
}



void Sampler::precomputeColorF()
{
	unsigned nSamples = SQRT_SAMPLES_NUM * SQRT_SAMPLES_NUM;
	unsigned nFuncs = BAND_NUM * BAND_NUM;

	SHEvalFunc SHEval[] = { SHEval3, SHEval3, SHEval3, SHEval3, SHEval4, SHEval5, SHEval6, SHEval7, SHEval8, SHEval9, SHEval10 };
	LFLOAT tShValues[BAND_NUM * BAND_NUM];



	LFLOAT original = 0.0, res = 0.0;
	LFLOAT approximateCoeffs[BAND_NUM * BAND_NUM];

	memset(approximateCoeffs, 0, sizeof(LFLOAT)* nFuncs);

	for (unsigned lobeIdx = 0; lobeIdx < LOBE_NUM; ++lobeIdx)
	{
		LFLOAT x = lobesF[lobeIdx][0], y = lobesF[lobeIdx][1], z = lobesF[lobeIdx][2];
		SHEval[BAND_NUM](x, y, z, tShValues);

		for (int l = 0; l < nFuncs; ++l)
		{
			approximateCoeffs[l] += lobesF[lobeIdx][l + 3] * tShValues[l];
		}
	}


	for (unsigned sampleIdx = 0; sampleIdx < nSamples; ++sampleIdx)
	{
		SAMPLE &curSample = samples[sampleIdx];
		curSample.approximateColorF = 0.0;
		original = 0.0;

		for (unsigned l = 0; l < nFuncs; ++l)
		{
			curSample.approximateColorF += approximateCoeffs[l] * curSample.shValues[l];
			original += coeffsF[l] * curSample.shValues[l];
		}

		LFLOAT tmp = curSample.colorF - curSample.approximateColorF;
		res += tmp * tmp;

		//curSample.approximateColorF = original;
	}
	res /= nSamples;
	res = sqrt(res);
	cout << "RMS: " << res << endl;
}

void Sampler::generateLobeF(Directions &dirs)
{
	unsigned nSamples = SQRT_SAMPLES_NUM * SQRT_SAMPLES_NUM;
	unsigned nFuncs = BAND_NUM * BAND_NUM;

	SHEvalFunc SHEval[] = { SHEval3, SHEval3, SHEval3, SHEval3, SHEval4, SHEval5, SHEval6, SHEval7, SHEval8, SHEval9, SHEval10 };


	LFLOAT *tcoeffs = new LFLOAT[nFuncs];
	LFLOAT *tlm = new LFLOAT[nFuncs];//先存暂shValues，然后通过与gil*  得到tlm

	memcpy(tcoeffs, coeffsF, nFuncs * sizeof(LFLOAT));

	BFGS bfgs;

	for (int i = 0; i < nSamples; ++i)
	{
		BFGS::samples[i] = samples[i];
	}


	for (int lobeIdx = 0; lobeIdx < LOBE_NUM; ++lobeIdx)
	{
		memcpy(bfgs.tlm, tcoeffs, nFuncs * sizeof(LFLOAT));
		lbfgsfloatval_t minFx = (lbfgsfloatval_t)INT_MAX;
		//cout << "#######################lobe: " << lobeIdx << endl;
		for (int dirIdx = 0; dirIdx < DIR_NUM; ++dirIdx)
		{
			//cout << "dir: " << dirIdx << endl;
			DIR direction = dirs.directions[dirIdx];

			bfgs.init();

			bfgs.x[0] = direction.xyz.x;
			bfgs.x[1] = direction.xyz.y;
			bfgs.x[2] = direction.xyz.z;


			unsigned index = 3;

			for (int l = 0; l < BAND_NUM; ++l)
			{
				int m;
				LFLOAT numerator = 0;
				LFLOAT denominator = 0;

				for (m = -l; m <= l; ++m)
				{
					numerator += coeffsF[INDEX(l, m)] * direction.shValues[INDEX(l, m)];
				}
				denominator = (2.0 * l + 1) / (4 * M_PI);
				for (int j = 0; j < 2 * l + 1; ++j)
				{
					bfgs.x[index + j] = numerator / denominator;
				}
				index += 2 * l + 1;
			}

			lbfgsfloatval_t fx;
			bfgs.bfgs(&fx);

			//cout << fx << "  " << minFx << endl;
			if (fx < minFx)
			{
				LFLOAT x = bfgs.x[0], y = bfgs.x[1], z = bfgs.x[2];

				LFLOAT D = sqrt(x * x + y * y + z * z);
				bfgs.x[0] = x / D;
				bfgs.x[1] = y / D;
				bfgs.x[2] = z / D;

				minFx = fx;
				for (unsigned i = 0; i < nFuncs + 3; ++i)
				{
					lobesF[lobeIdx][i] = bfgs.x[i];
				}
			}
		}



		SHEval[BAND_NUM](lobesF[lobeIdx][0], lobesF[lobeIdx][1], lobesF[lobeIdx][2], tlm);
		for (unsigned i = 0; i < nFuncs; ++i)
		{
			tlm[i] *= lobesF[lobeIdx][i + 3];

			tcoeffs[i] -= tlm[i];
		}

	}
}

unsigned Sampler::size()
{
	return this->samples.size();
}

SAMPLE &Sampler::operator[](unsigned int index)
{
	assert(index >= 0 && index < samples.size());
	return samples[index];
}