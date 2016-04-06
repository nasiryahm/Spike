// Synapse Class Header
// Synapse.h
//
//	Author: Nasir Ahmad
//	Date: 7/12/2015

#ifndef Synapse_H
#define Synapse_H
#include "Structs.h"
class Synapse{
public:
	// Constructor/Destructor
	Synapse();
	~Synapse();
	// Variables;
	int numconnections;
	// STDP
	struct stdp_struct stdp_vars;
	void SetSTDP(float w_max_new,
				float a_minus_new,
				float a_plus_new,
				float tau_minus_new,
				float tau_plus_new);
	// Full Matrices
	int* presyns;
	int* postsyns;
	float* weights;
	float* lastactive;
	int* delays;
	int* stdp;
	// Synapse Functions
	void AddConnection(int prepop, 
						int postpop, 
						int* popNums,
						int** pop_shapes, 
						int connectivity_type,
						float weightrange[2],
						int delayrange[2],
						bool stdpswitch,
						float parameter,
						float parameter_two);
};
// GAUSS random number generator
// double randn (double mu, double sigma);
#endif