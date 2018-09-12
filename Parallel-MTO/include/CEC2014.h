#ifndef __CEC2014_H__
#define __CEC2014_H__
#include "random.h"
#include <math.h>
#include <algorithm>
#include "config.h"
using namespace std;
#define INF 1.0e99
#define EPS 1.0e-14
#define E  2.7182818284590452353602874713526625
#define PI 3.1415926535897932384626433832795029
class  CEC2014
{
private:
	Random 							random_;
	vector<real> 					shift_;
	vector< vector<real> > 			rotation_;

	int								func_id_;
	int            					dim_;
	real 							correlation_;
	real  							Spearman(int test_num, int sample_num);
	int  							CalRotation();
	int  							CalShift(int option);

	real 							Ackley(vector<real> x, double rate, int shift_flag, int rotate_flag);
	real 							Rastrigin(vector<real> x, double rate, int shift_flag, int rotate_flag); 
	real 							Weierstrass(vector<real> x, double rate, int shift_flag, int rotate_flag); 
	real 							Griewank(vector<real> x, double rate, int shift_flag, int rotate_flag); 
	vector<real> 					ShiftAndRotate(vector<real> x, double shift_rate, int shift_flag, int rotate_flag);
public:
	CEC2014();			
	~CEC2014();
	int								Initilize(int func_id, int dim, int option, int seed);
	int								Unitilize();
	real							EvaluateFitness(const vector<double> & elements);
	real 							EvaluateFitnessWoShift(const vector<double> & elements);
	real 							Correlation();
	real 							Shift(int element_ID);
	real 							Rotation(int element_ID_1, int element_ID_2);

};

#endif
