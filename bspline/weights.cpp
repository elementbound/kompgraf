#include <iostream>
#include <cstdlib> //atoi

double bspline_weight(double u, unsigned point_id, unsigned order)
{
	unsigned& i = point_id;
	
	if(order == 0)
		return (i <= u && u < i+1) ? 1.0 : 0.0;
	else 
		return (u-i)/(i+order - i) * bspline_weight(u, i, order-1) +
			   (i+order+1 - u)/(i+order+1 - i+1) * bspline_weight(u, i+1, order-1);
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
	
	std::cout.setf(std::ios::fixed);
	std::cout.precision(4);
	
	for(unsigned it=0; it<detail; it++)
	{
		double t = it/double(detail-1) * (order+1);
		std::cout << "b(" << t << "): ";
		
		for(unsigned i=0; i<order+1; i++)
		{
			std::cout << bspline_weight(t, i, order) << "*P[" << i << "]";
			if(i+1 < order+1) std::cout << " + ";
		}
		std::cout << std::endl;
	}
	
	return 0;
}