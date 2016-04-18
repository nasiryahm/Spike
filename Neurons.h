//	Neurons Class Header
//	Neurons.h
//
//	Author: Nasir Ahmad
//	Date: 7/12/2015
//
//  Adapted from NeuronPopulations by Nasir Ahmad and James Isbister
//	Date: 6/4/2016

#ifndef Neurons_H
#define Neurons_H

#include <cuda.h>
#include <stdio.h>

#include "Connections.h"


struct neuron_parameters_struct {
	neuron_parameters_struct() { }

};


class Neurons{
public:
	// Constructor/Destructor
	Neurons();
	~Neurons();

	int total_number_of_neurons;
	int total_number_of_groups;

	float * d_last_spike_time;
	float* d_current_injections;

	int **group_shapes;
	int *last_neuron_indices_for_each_group;

	int number_of_neurons_in_new_group;

	dim3 number_of_neuron_blocks_per_grid;
	dim3 threads_per_block;


	// Functions
	virtual int AddGroup(neuron_parameters_struct * group_params, int group_shape[2]);
	virtual void initialise_device_pointers();
	virtual void reset_neuron_variables_and_spikes();

	void reset_device_current_injections();

	virtual void set_threads_per_block_and_blocks_per_grid(int threads);

};

#endif