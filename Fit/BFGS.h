#ifndef __BFGS_H__
#define __BFGS_H__

#include "lbfgs.h"
#include "Global.h"
#include "Sampler.h"


class BFGS
{
public:
	BFGS(){}
	static void bfgs(lbfgsfloatval_t *fx);

	static void init();

	static lbfgsfloatval_t evaluate(
		void *instance,
		const lbfgsfloatval_t *x,
		lbfgsfloatval_t *g,
		const int n,
		const lbfgsfloatval_t step
		);

	static int n;
	static lbfgsfloatval_t *x;
	static lbfgs_parameter_t param;
	static SAMPLE *samples;
	static LFLOAT *tlm;
};

#endif