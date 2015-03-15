#include <iostream>
#include <cstdlib> //atoi
#include <vector>
#include <algorithm> //generate

inline int fcmp(double a, double b){return ((b-a)<1e-8) - ((a-b)<1e-8);}

double bspline_weight(double t, unsigned point_id, unsigned order, const std::vector<double>& u)
{
	unsigned& i = point_id;
	
	if(order == 0)
		return (u[i] <= t && t < u[i+1]);
		//return (fcmp(u[i],t) >= 0 && fcmp(t, u[i+1]) < 0) ? 1.0 : 0.0;
	else 
		return (t-u[i])/double(u[i+order] - u[i]) * bspline_weight(t, i, order-1, u) +
			   (u[i+order+1] - t)/double(u[i+order+1] - u[i+1]) * bspline_weight(t, i+1, order-1, u);
}

int main(int argc, char** argv)
{
	if(argc < 2)
	{
		std::cout << "Usage: " << argv[0] << " <order> <detail>" << std::endl;
		return 1;
	}
	
	unsigned order = atoi(argv[1]);
	unsigned detail = atoi(argv[2]);
	std::vector<double> knot_values;
	
	knot_values.resize(order+2);
	std::generate(knot_values.begin(), knot_values.end(), 
		[]() -> double {
			static unsigned i=0;
			return i++;
		}
	);
	
	std::cout << "Knot values: ";
	for(double t : knot_values) std::cout << t << ' ';
	std::cout << std::endl;
	
	std::cout.setf(std::ios::fixed);
	std::cout.precision(4);
	
	for(unsigned it=0; it<detail; it++)
	{
		double t = 1.0 + it/double(detail-1) * order;
		std::cout << "b(" << t << "): ";
		
		for(unsigned i=0; i<order+1; i++)
		{
			std::cout << bspline_weight(t, i, order, knot_values) << "*P[" << i << "]";
			if(i+1 < order+1) std::cout << " + ";
		}
		std::cout << std::endl;
	}
	
	return 0;
}