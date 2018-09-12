#include "CEC2014.h"

CEC2014::CEC2014()
{


}

CEC2014::~CEC2014()
{

}
real CEC2014::EvaluateFitnessWoShift(const vector<real> & elements)
{
	real fitness_value = 0;

	switch (func_id_)
	{
	case 1:
		fitness_value = Ackley(elements, 0.32, 0, 1);
		break;
	case 2:
		fitness_value = Weierstrass(elements,  0.5 / 100.0, 0, 1);
		break;
	case 3:
		fitness_value = Griewank(elements, 6.0, 0, 1);
		break;
	case 4:
		fitness_value = Rastrigin(elements, 5.0 / 100.0, 0, 1);
		break;
	default:
		printf("\nError: There are only 4 test functions in this test suite!\n");
		fitness_value = -1.0;
		break;
	}
	return fitness_value;
}
double CEC2014::EvaluateFitness(const vector<double> & elements)
{
	real fitness_value = 0;

	switch (func_id_)
	{
	case 1:
		fitness_value = Ackley(elements, 0.32, 1, 1);
		break;
	case 2:
		fitness_value = Weierstrass(elements,  0.5 / 100.0, 1, 1);
		break;
	case 3:
		fitness_value = Griewank(elements, 6.0, 1, 1);
		break;
	case 4:
		fitness_value = Rastrigin(elements, 5.0 / 100.0, 1, 1);
		break;
	default:
		printf("\nError: There are only 4 test functions in this test suite!\n");
		fitness_value = -1.0;
		break;
	}
	return fitness_value;
}


int	CEC2014::Unitilize()
{
	shift_.clear();
	rotation_.clear();

	return 0;
}


int CEC2014::Initilize(int func_id, int dim, int option, int seed)
{
	srand(seed);
	correlation_ = 0;

	func_id_ = func_id;
	dim_ = dim;
	vector<real> tmp_rotate(dim_, 0);
	for (int i = 0; i < dim_; i++)
	{
		shift_.push_back(0);
		rotation_.push_back(tmp_rotate);
	}
	CalRotation();

	CalShift(option);

	return 0;
}
int CEC2014::CalShift(int option)
{
	int test_num = 1;
	int sample_num = 1000;

	double low = 0.001;
	double high = 1;
	double shift_bound = 5;
	for(int i = 0; i < dim_; i++)
		shift_[i] = random_.RandRealUnif(-shift_bound, shift_bound);
	// while(correlation_ > 0.8 || correlation_ < 0.2)
	// {
	// 	shift_rate = random_.RandRealUnif(low, high);

	// 	for(int i = 0; i < dim_; i++)
	// 		shift_[i] = random_.RandRealUnif(shift_rate * (-100), shift_rate * 100);

	// 	correlation_ = Spearman(test_num, sample_num);
	// 	if (correlation_ < 0.2)
	// 		high = high / 1.1;
	// 	if (correlation_ > 0.8)
	// 		low = low * 1.1;
	// 	if (low >= high)
	// 	{
	// 		printf("error: low (%lf) >= high (%lf)\n", low, high);
	// 		break;
	// 	}
	// }

	return 0;
}
int CEC2014::CalRotation()
{
    real prod = 0;
    vector<real> gvect(dim_ * dim_, 0);
    vector<real> uniftmp(2 * dim_ * dim_, 0);

    for (int i = 0; i < 2 * dim_ * dim_; i++)
    	uniftmp[i] = random_.RandRealUnif(0, 1);

    for (int i = 0; i < dim_ * dim_; i++)
    {
        gvect[i] = sqrt(-2 * log(uniftmp[i])) * cos(2 * PI * uniftmp[dim_ * dim_ + i]);
        if (gvect[i] == 0.0)
            gvect[i] = 1e-99;
    }

    for (int i = 0; i < dim_; i++)
    {
        for (int j = 0; j < dim_; j++)
        {
            rotation_[i][j] = gvect[j * dim_ + i];
        }
    }

    for (int i = 0; i < dim_; i++)
    {
        for (int j = 0; j < i; j++)
        {
            prod = 0;
            for (int k = 0; k < dim_; k++)
            {
                prod += rotation_[k][i] * rotation_[k][j];
            }
            for (int k = 0; k < dim_; k++)
            {
                rotation_[k][i] -= prod * rotation_[k][j];
            }
        }
        prod = 0;
        for (int k = 0; k < dim_; k++)
        {
            prod += rotation_[k][i] * rotation_[k][i];
        }
        for (int k = 0; k < dim_; k++)
        {
            rotation_[k][i] /= sqrt(prod);
        }
    }
    return 0;
}


real CEC2014::Correlation()
{
	return correlation_;
}
real CEC2014::Shift(int element_ID)
{
	return shift_[element_ID];
}
real CEC2014::Rotation(int element_ID_1, int element_ID_2)
{
	return rotation_[element_ID_1][element_ID_2];
}

real CEC2014::Spearman(int test_num, int sample_num)
{
	real cor = 0;
	for(int i = 0; i < test_num; i++)
	{
		vector<real> y0, y1, rank0, rank1;
		vector<int> d;

		for(int j = 0; j < sample_num; j++)
		{
			vector<real> elements;
			for(int k = 0; k < dim_; k++)
				elements.push_back(random_.RandRealUnif(-100, 100));

			y0.push_back(EvaluateFitnessWoShift(elements));
			y1.push_back(EvaluateFitness(elements));
		}
		for(int j = 0; j < sample_num; j++)
		{
			rank0.push_back(0);
			rank1.push_back(0);
			for(int k = 0; k < sample_num; k++)
			{
				if (y0[j] > y0[k])
					rank0[j] = rank0[j] + 1;
				if(y0[j] == y0[k] && j!= k)
					rank0[j] = rank0[j] + 0.5;
				if (y1[j] > y1[k])
					rank1[j] = rank1[j] + 1;
				if(y1[j] == y1[k] && j!= k)
					rank1[j] = rank1[j] + 0.5;
			}
		}


   		for(size_t j = 0; j < sample_num; j++)
   		{
   			d.push_back(abs(rank0[j] - rank1[j]));
   			d[j] = d[j] * d[j];
   		}

   		int sum = accumulate(d.begin(),d.end(), 0);
   		cor = 1 - 6.0 * sum / (sample_num * (sample_num * sample_num - 1.0) + 0.0);
   		if (cor < 0.2 || cor > 0.8)
   			return cor;	
	}
	return cor;
}

real CEC2014::Ackley(vector<real> x, real rate, int shift_flag, int rotate_flag) /* Ackley's  */
{
	real sum1 = 0.0, sum2 = 0.0, f = 0;

	vector<real> shifted_x;
	shifted_x = ShiftAndRotate(x, rate, shift_flag, rotate_flag); /* shift and rotate */

	for (int i = 0; i < dim_; i++)
	{
		sum1 += shifted_x[i] * shifted_x[i];
		sum2 += cos(2.0 * PI * shifted_x[i]);
	}
	sum1 = -0.2 * sqrt(sum1 / (real) dim_);
	sum2 /= (real) dim_;
	f = E - 20.0*exp(sum1) + 20.0 - exp(sum2);
	return f;
}


real CEC2014::Weierstrass(vector<real> x, real rate, int shift_flag, int rotate_flag) /* Weierstrass's  */
{
	int k_max = 20;
	real sum = 0, sum2 = 0,  a = 0.5, b = 3.0, f = 0;

	vector<real> shifted_x;
	shifted_x = ShiftAndRotate(x, rate, shift_flag, rotate_flag); /* shift and rotate */
	for (int i = 0; i < dim_; i++)
	{
		sum = 0.0;
		sum2 = 0.0;
		for (int j = 0; j <= k_max; j++)
		{
			sum += pow(a, j) * cos(2.0 * PI * pow(b, j) * (shifted_x[i] + 0.5));
			sum2 += pow(a, j) * cos(2.0 * PI * pow(b, j) * 0.5);
		}
		f += sum;
	}
	f -= dim_ * sum2;
	return f;
}


real CEC2014::Griewank(vector<real> x, real rate, int shift_flag, int rotate_flag) /* Griewank's  */
{
	real s = 0.0, p = 1.0, f = 0.0;

	vector<real> shifted_x;
	shifted_x = ShiftAndRotate(x, rate, shift_flag, rotate_flag); /* shift and rotate */

	for (int i = 0; i < dim_; i++)
	{
		s += shifted_x[i] * shifted_x[i];
		p *= cos(shifted_x[i] / sqrt(1.0 + i));
	}
	f = 1.0 + s / 4000.0 - p;
	return f;
}

real CEC2014::Rastrigin(vector<real> x, real rate, int shift_flag, int rotate_flag) /* Rastrigin's  */
{
	real f = 0.0;
	vector<real> shifted_x;
	shifted_x = ShiftAndRotate(x, rate, shift_flag, rotate_flag); /* shift and rotate */

	for (int i = 0; i < dim_; i++)
	{
		f += (shifted_x[i] * shifted_x[i] - 10.0*cos(2.0*PI*shifted_x[i]) + 10.0);
	}
	return f;
}


vector<real> CEC2014::ShiftAndRotate(vector<real> x, real shift_rate, int shift_flag, int rotate_flag)
{
	vector<real> shifted_x(dim_, 0);

	if(shift_flag == 1)
	{
		for (int i = 0; i < dim_; i++)
			shifted_x[i] = (x[i] - shift_[i]) * shift_rate;
	}
	else
	{
		for (int i = 0; i < dim_; i++)
			shifted_x[i] = x[i] * shift_rate;
	}
	if(rotate_flag == 1)
	{
		vector<real> tmp_shift_x(dim_, 0);
		for (int i = 0; i < dim_; i++)
		{
			for (int j = 0; j < dim_; j++)
				tmp_shift_x[i] += rotation_[i][j] * shifted_x[j];
		}
		shifted_x = tmp_shift_x;
	}

	return shifted_x;
}
