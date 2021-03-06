//  WeightDependentSTDPPlasticity STDP Class C++
//  WeightDependentSTDPPlasticity.cu
//
//  Author: Nasir Ahmad
//  Date: 03/10/2016


#include "WeightDependentSTDPPlasticity.hpp"
#include "../Helpers/TerminalHelpers.hpp"

WeightDependentSTDPPlasticity::WeightDependentSTDPPlasticity(SpikingSynapses* synapses, SpikingNeurons* neurons, SpikingNeurons* input_neurons, stdp_plasticity_parameters_struct* stdp_parameters){
  stdp_params = (weightdependent_stdp_plasticity_parameters_struct *)stdp_parameters;
}

WeightDependentSTDPPlasticity::~WeightDependentSTDPPlasticity() {
}

// Run the STDP
void WeightDependentSTDPPlasticity::state_update(unsigned int current_time_in_timesteps, float timestep){
  backend()->apply_stdp_to_synapse_weights(current_time_in_timesteps, timestep);
}

SPIKE_MAKE_INIT_BACKEND(WeightDependentSTDPPlasticity
);
