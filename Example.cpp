// An Example Model for running the SPIKE simulator
//
//	Author: Nasir Ahmad
//	Date: 16/03/2016

//  Adapted by Nasir Ahmad and James Isbister
//	Date: 23/3/2016

// To create the executable for this network, run:
// make FILE="Example" model


#include "Simulator.h"
#include "Constants.h"
#include "Neurons.h"
// The function which will autorun when the executable is created
int main (int argc, char *argv[]){
	// Set the timestep of the simulation as required (timestep is measure in seconds)
	float timest = 0.001;
	// Create an instance of the Simulator and set the timestep
	Simulator simulator;
	simulator.SetTimestep(timest);
	
	simulator.SetNeuronType(new Neurons());

	neuron_struct test_params;
	test_params.parama = 0.02f;
	test_params.paramb = -0.01f;
	test_params.paramc = -55.0f;
	test_params.paramd = 6.0f;

	int test_shape[] = {1000, 1};

	int test_neuron_group_1_id = simulator.AddNeuronGroup(test_params, test_shape);
	int test_neuron_group_2_id = simulator.AddNeuronGroup(test_params, test_shape);

	float test_weight_range[] = {2.50f, 5.0f};
	float test_delay_range[] = {timest, 50.0f*pow(10, -3)};
	bool stdp_on = true;

	simulator.AddConnectionGroup(test_neuron_group_1_id, test_neuron_group_2_id, CONNECTIVITY_TYPE_ALL_TO_ALL, test_weight_range, test_delay_range, stdp_on);

	// /*

	// 		CREATING THE MODEL

	// */
	// // Parameter sets
	// // Parameters for a set of example Sub-Cortical, Cortical and Inhibitory Neurons
	// neuron_struct paramSubCort;
	// paramSubCort.parama = 0.02f;
	// paramSubCort.paramb = -0.01f;
	// paramSubCort.paramc = -55.0f;
	// paramSubCort.paramd = 6.0f;

	// neuron_struct paramCort;
	// paramCort.parama = 0.01f;
	// paramCort.paramb = 0.2f;
	// paramCort.paramc = -65.0f;
	// paramCort.paramd = 8.0f;

	// neuron_struct paramInh;
	// paramCort.parama = 0.02f;
	// paramCort.paramb = 0.25f;
	// paramCort.paramc = -55.0f;
	// paramCort.paramd = 0.05f;
	// // We must also create a neuron parameter structure for our rate coded neurons
	// // Rate is measured in average spikes per second.
	// neuron_struct poisson_params;
	// poisson_params.rate = 30.0f;
    
 //    // Population Shapes
 //    int input_population_shape[] = {1000, 1};
 //    int intermediate_population_shape[] = {1000, 1};
 //    int penultimate_population_shape[] = {1000, 1};
 //    int intermediate_inhibitory_population_shape[] = {1000, 1};
 //    int penultimate_inhibitory_population_shape[] = {1000, 1};
 //    int output_population_shape[] = {100, 1};
 //    int output_inhibitory_population_shape[] = {100, 1};
	// // Creating populations
	// // Sub-Cortical
	// // Syntax for neuron creation is (number_of_neurons, neuron_type, neuron_parameters)
	// int INPUTLAYER = simulator.CreateNeurons(NEURON_TYPE_POISSON, poisson_params, input_population_shape);
	// // Create some intermediary layers
	// int INTERMEDIATE = simulator.CreateNeurons(NEURON_TYPE_IZHIKEVICH, paramSubCort, intermediate_population_shape);
	// int PENULTIMATE = simulator.CreateNeurons(NEURON_TYPE_IZHIKEVICH, paramSubCort, penultimate_population_shape);
	// // Inhibitory Neurons
	// int INTERMEDIATE_INHIB = simulator.CreateNeurons(NEURON_TYPE_IZHIKEVICH, paramInh, intermediate_inhibitory_population_shape);
	// int PENULTIMATE_INHIB = simulator.CreateNeurons(NEURON_TYPE_IZHIKEVICH, paramInh, penultimate_inhibitory_population_shape);
	// // Cortical Neurons
	// int OUTPUTLAYER = simulator.CreateNeurons(NEURON_TYPE_IZHIKEVICH, paramCort, output_population_shape);
	// int OUTPUTLAYER_INHIB = simulator.CreateNeurons(NEURON_TYPE_IZHIKEVICH, paramInh, output_inhibitory_population_shape);

	// // Connect Populations
	// // Connection profiles
	// // Excitatory Weights ranges (if the values are the same, the weights are not set randomly)
	// float INPUT_TO_INTER_weights[] = {2.50f, 5.0f};
	// float INTER_TO_PEN_weights[] = {35.0f, 35.0f};
	// float PEN_TO_OUTPUT_weights[] = {5.0f, 50.0f};
	// // Inhibitory Weights
	// float EXC_TO_INH_weights[] = {15.0f, 15.0f};
	// float INH_TO_EXC_weights[] = {-10.0f, -10.0f};
	// // Delays
	// float DefaultDelay[] = {timest, timest};
	// float CortDelay[] = {timest, 50.0f*pow(10, -3)};

	// // Excitatory Connections
	// // Syntax is (input_layer_id, output_layer_id, connectivity_type, weight_vec, delay_vec, stdp_on, parameter)
	// // The parameter is only necessary for random or gaussian connectivites
	// // simulator.CreateConnection(INPUTLAYER, INTERMEDIATE, CONNECTIVITY_TYPE_ALL_TO_ALL, INPUT_TO_INTER_weights, DefaultDelay, false);
	// // simulator.CreateConnection(INTERMEDIATE, PENULTIMATE, CONNECTIVITY_TYPE_ONE_TO_ONE, INTER_TO_PEN_weights, DefaultDelay, false);
	// // simulator.CreateConnection(PENULTIMATE, OUTPUTLAYER, CONNECTIVITY_TYPE_RANDOM, PEN_TO_OUTPUT_weights, CortDelay, true, 0.75f);
	// // // Inhibitory Connections
	// // // Excitation for the inhibitory neurons
	// // simulator.CreateConnection(INTERMEDIATE, INTERMEDIATE_INHIB, CONNECTIVITY_TYPE_ONE_TO_ONE, EXC_TO_INH_weights, DefaultDelay, false);
	// // simulator.CreateConnection(PENULTIMATE, PENULTIMATE_INHIB, CONNECTIVITY_TYPE_ONE_TO_ONE, EXC_TO_INH_weights, DefaultDelay, false);
	// // simulator.CreateConnection(OUTPUTLAYER, OUTPUTLAYER_INHIB, CONNECTIVITY_TYPE_ONE_TO_ONE, EXC_TO_INH_weights, DefaultDelay, false);
	// // // Inhibition from inhibitory neurons to excitatory
	// // simulator.CreateConnection(INTERMEDIATE_INHIB, INTERMEDIATE, CONNECTIVITY_TYPE_ALL_TO_ALL, INH_TO_EXC_weights, DefaultDelay, false);
	// // simulator.CreateConnection(PENULTIMATE_INHIB, PENULTIMATE, CONNECTIVITY_TYPE_ALL_TO_ALL, INH_TO_EXC_weights, DefaultDelay, false);
	// // simulator.CreateConnection(OUTPUTLAYER_INHIB, OUTPUTLAYER, CONNECTIVITY_TYPE_ALL_TO_ALL, INH_TO_EXC_weights, DefaultDelay, false);

	/*

			RUNNING THE SIMULATION

	*/

	// Run the actual simulator!
	float totaltime = 1.25f;
	int epochs = 1;
	bool outputspikes = true;
	simulator.Run(totaltime, epochs, outputspikes);

	// Since outputspikes was set to true, the spike times of every neuron will be output along side the network connectivity and weights.
	// All outputs are placed in a results folder in the current directory.
	return 1;
}