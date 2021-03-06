#include "SpikingActivityMonitor.hpp"
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include "../Helpers/TerminalHelpers.hpp"
#include <string>
#include <time.h>
using namespace std;

// SpikingActivityMonitor Constructor
SpikingActivityMonitor::SpikingActivityMonitor(SpikingNeurons * neurons_parameter){
  neurons = neurons_parameter;

  // Variables
  size_of_device_spike_store = 0;
  total_number_of_spikes_stored_on_host = 0;

  // Host Pointers
  advanced_parameters = new spike_monitor_advanced_parameters();
  neuron_ids_of_stored_spikes_on_host = nullptr;
  spike_times_of_stored_spikes_on_host = nullptr;

  // Private Host Pointeres
  reset_neuron_ids = nullptr;
  reset_neuron_times = nullptr;
  
  // Initialize the single sized value
  total_number_of_spikes_stored_on_device = (int*)malloc(sizeof(int));
  total_number_of_spikes_stored_on_device[0] = 0;

}


// SpikingActivityMonitor Destructor
SpikingActivityMonitor::~SpikingActivityMonitor() {
  free(neuron_ids_of_stored_spikes_on_host);
  free(spike_times_of_stored_spikes_on_host);
  free(total_number_of_spikes_stored_on_device);

  free(reset_neuron_ids);
  free(reset_neuron_times);
}

void SpikingActivityMonitor::prepare_backend_early() {
  size_of_device_spike_store = advanced_parameters->device_spike_store_size_multiple_of_total_neurons * neurons->total_number_of_neurons;
  allocate_pointers_for_spike_store();
}

void SpikingActivityMonitor::allocate_pointers_for_spike_store() {

  reset_neuron_ids = (int *)malloc(sizeof(int)*size_of_device_spike_store);
  reset_neuron_times = (float *)malloc(sizeof(float)*size_of_device_spike_store);
  for (int i=0; i < size_of_device_spike_store; i++){
    reset_neuron_ids[i] = -1;
    reset_neuron_times[i] = -1.0f;
  }
}

void SpikingActivityMonitor::reset_state() {
  // Reset the spike store
  // Host values
  total_number_of_spikes_stored_on_host = 0;
  total_number_of_spikes_stored_on_device[0] = 0;
  // Free/Clear Device stuff
  // Reset the number on the device
  backend()->reset_state();

  /*
  // Free malloced host stuff
  free(neuron_ids_of_stored_spikes_on_host);
  free(spike_times_of_stored_spikes_on_host);
  neuron_ids_of_stored_spikes_on_host = nullptr;
  spike_times_of_stored_spikes_on_host = nullptr;
  */
}


void SpikingActivityMonitor::copy_spikes_from_device_to_host_and_reset_device_spikes_if_device_spike_count_above_threshold(unsigned int current_time_in_timesteps, float timestep, bool force) {

  if (((current_time_in_timesteps % (advanced_parameters->number_of_timesteps_per_device_spike_copy_check / model->timestep_grouping)) == 0) || force){

    // Finally, we want to get the spikes back. Every few timesteps check the number of spikes:
    backend()->copy_spikecount_to_front();

    // Ensure that we don't have too many
    if (total_number_of_spikes_stored_on_device[0] > size_of_device_spike_store){
      print_message_and_exit("Spike recorder has been overloaded! Reduce threshold.");
    }

    // Deal with them!
    if ((total_number_of_spikes_stored_on_device[0] >= (advanced_parameters->proportion_of_device_spike_store_full_before_copy * size_of_device_spike_store)) ||  force){

      // Reallocate host spike arrays to accommodate for new device spikes.
      neuron_ids_of_stored_spikes_on_host = (int*)realloc(neuron_ids_of_stored_spikes_on_host, sizeof(int)*(total_number_of_spikes_stored_on_host + total_number_of_spikes_stored_on_device[0]));
      spike_times_of_stored_spikes_on_host = (float*)realloc(spike_times_of_stored_spikes_on_host, sizeof(float)*(total_number_of_spikes_stored_on_host + total_number_of_spikes_stored_on_device[0]));
      // Copy device spikes into correct host array location
      backend()->copy_spikes_to_front();

      total_number_of_spikes_stored_on_host += total_number_of_spikes_stored_on_device[0];


      // Reset device spikes
      backend()->reset_state();
      total_number_of_spikes_stored_on_device[0] = 0;
    }
  }
}


void SpikingActivityMonitor::state_update(unsigned int current_time_in_timesteps, float timestep){
  backend()->collect_spikes_for_timestep(current_time_in_timesteps, timestep);
  copy_spikes_from_device_to_host_and_reset_device_spikes_if_device_spike_count_above_threshold(current_time_in_timesteps, timestep);
}

void SpikingActivityMonitor::final_update(unsigned int current_time_in_timesteps, float timestep){
  copy_spikes_from_device_to_host_and_reset_device_spikes_if_device_spike_count_above_threshold(current_time_in_timesteps, timestep, true);
  printf(" Number of Spikes Recorded: %d\n", total_number_of_spikes_stored_on_host);
}


void SpikingActivityMonitor::save_spikes_as_txt(string path, string prefix){
  ofstream spikeidfile, spiketimesfile;

  // Open output files
  spikeidfile.open((path + "/" + prefix + "SpikeIDs.txt"), ios::out | ios::binary);
  spiketimesfile.open((path + "/" + prefix + "SpikeTimes.txt"), ios::out | ios::binary);

  // Send the data
  for (int i = 0; i < total_number_of_spikes_stored_on_host; i++) {
    spikeidfile << neuron_ids_of_stored_spikes_on_host[i] << endl;
    spiketimesfile << spike_times_of_stored_spikes_on_host[i] << endl;
  }
  // Close the files
  spikeidfile.close();
  spiketimesfile.close();
}

void SpikingActivityMonitor::save_spikes_as_binary(string path, string prefix){
  ofstream spikeidfile, spiketimesfile;

  // Open output files
  spikeidfile.open((path + "/" + prefix + "SpikeIDs.bin"), ios::out | ios::binary);
  spiketimesfile.open((path + "/" + prefix + "SpikeTimes.bin"), ios::out | ios::binary);

  // Send the data
  spikeidfile.write((char *)neuron_ids_of_stored_spikes_on_host, total_number_of_spikes_stored_on_host*sizeof(int));
  spiketimesfile.write((char *)spike_times_of_stored_spikes_on_host, total_number_of_spikes_stored_on_host*sizeof(float));
  // Close the files
  spikeidfile.close();
  spiketimesfile.close();
}


SPIKE_MAKE_INIT_BACKEND(SpikingActivityMonitor);
