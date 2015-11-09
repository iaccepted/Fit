#ifndef __BFGS_H__
#define __BFGS_H__

#include "lbfgs.h"
#include "Global.h"
#include "Sampler.h"


class BFGS
{
public:
	BFGS(){}
	void bfgs(lbfgsfloatval_t *fx);

	void init();

	static lbfgsfloatval_t evaluate(
		void *instance,
		const lbfgsfloatval_t *x,
		lbfgsfloatval_t *g,
		const int n,
		const lbfgsfloatval_t step
		);

	int n;
	lbfgsfloatval_t *x;
	lbfgs_parameter_t param;
	//static std::vector<SAMPLE> samples;
	static SAMPLE *samples;
	static LFLOAT *tlm;
};

#endif