// -*- mode: c++ -*-
#include "Spike/Backend/CUDA/Neurons/LIFSpikingNeurons.hpp"

SPIKE_EXPORT_BACKEND_TYPE(CUDA, LIFSpikingNeurons);

namespace Backend {
  namespace CUDA {
    namespace INLINE_LIF {
      #include "Spike/Backend/CUDA/InlineDeviceFunctions.hpp"
    }

    LIFSpikingNeurons::~LIFSpikingNeurons() {
      CudaSafeCall(cudaFree(membrane_potentials_v));
      CudaSafeCall(cudaFree(membrane_time_constants_tau_m));
      CudaSafeCall(cudaFree(membrane_resistances_R));
      CudaSafeCall(cudaFree(thresholds_for_action_potential_spikes));
      CudaSafeCall(cudaFree(resting_potentials_v0));
      CudaSafeCall(cudaFree(after_spike_reset_potentials_vreset));
      CudaSafeCall(cudaFree(background_currents));
      CudaSafeCall(cudaFree(refractory_periods));
      CudaSafeCall(cudaFree(refraction_counter));
    }

    void LIFSpikingNeurons::allocate_device_pointers() {
      CudaSafeCall(cudaMalloc((void **)&membrane_potentials_v, sizeof(float)*frontend()->total_number_of_neurons));
      CudaSafeCall(cudaMalloc((void **)&thresholds_for_action_potential_spikes, sizeof(float)*frontend()->spiking_thresholds_vthresh.size()));
      CudaSafeCall(cudaMalloc((void **)&resting_potentials_v0, sizeof(float)*frontend()->resting_potentials_v0.size()));
      CudaSafeCall(cudaMalloc((void **)&after_spike_reset_potentials_vreset, sizeof(float)*frontend()->after_spike_reset_potentials_vreset.size()));
      CudaSafeCall(cudaMalloc((void **)&membrane_time_constants_tau_m, sizeof(float)*frontend()->membrane_time_constants_tau_m.size()));
      CudaSafeCall(cudaMalloc((void **)&membrane_resistances_R, sizeof(float)*frontend()->membrane_resistances_R.size()));
      CudaSafeCall(cudaMalloc((void **)&refractory_periods, sizeof(float)*frontend()->refractory_periods.size()));
      CudaSafeCall(cudaMalloc((void **)&background_currents, sizeof(int)*frontend()->background_currents.size()));
      
      
      CudaSafeCall(cudaMalloc((void **)&neuron_labels, sizeof(int)*frontend()->total_number_of_neurons));
      CudaSafeCall(cudaMalloc((void **)&refraction_counter, sizeof(int)*frontend()->total_number_of_neurons));
      CudaSafeCall(cudaFree(d_neuron_data));
      CudaSafeCall(cudaMalloc((void **)&d_neuron_data, sizeof(lif_spiking_neurons_data_struct)));
    }

    void LIFSpikingNeurons::copy_constants_to_device() {
      CudaSafeCall(cudaMemcpy(thresholds_for_action_potential_spikes,
                              frontend()->spiking_thresholds_vthresh.data(),
                              sizeof(float)*frontend()->spiking_thresholds_vthresh.size(),
                              cudaMemcpyHostToDevice));

      CudaSafeCall(cudaMemcpy(after_spike_reset_potentials_vreset,
                              frontend()->after_spike_reset_potentials_vreset.data(),
                              sizeof(float)*frontend()->after_spike_reset_potentials_vreset.size(),
                              cudaMemcpyHostToDevice));

      CudaSafeCall(cudaMemcpy(membrane_time_constants_tau_m,
                              frontend()->membrane_time_constants_tau_m.data(),
                              sizeof(float)*frontend()->membrane_time_constants_tau_m.size(),
                              cudaMemcpyHostToDevice));
      
      CudaSafeCall(cudaMemcpy(membrane_resistances_R,
                              frontend()->membrane_resistances_R.data(),
                              sizeof(float)*frontend()->membrane_resistances_R.size(),
                              cudaMemcpyHostToDevice));
      
      CudaSafeCall(cudaMemcpy(background_currents,
                              frontend()->background_currents.data(),
                              sizeof(float)*frontend()->background_currents.size(),
                              cudaMemcpyHostToDevice));
      
      CudaSafeCall(cudaMemcpy(resting_potentials_v0,
                              frontend()->resting_potentials_v0.data(),
                              sizeof(float)*frontend()->resting_potentials_v0.size(),
                              cudaMemcpyHostToDevice));
      
      CudaSafeCall(cudaMemcpy(refractory_periods,
                              frontend()->refractory_periods.data(),
                              sizeof(int)*frontend()->refractory_periods.size(),
                              cudaMemcpyHostToDevice));
      
      CudaSafeCall(cudaMemcpy(neuron_labels,
                              frontend()->neuron_labels.data(),
                              sizeof(int)*frontend()->total_number_of_neurons,
                              cudaMemcpyHostToDevice));
    }

    void LIFSpikingNeurons::prepare() {
      SpikingNeurons::prepare();
      allocate_device_pointers();
      copy_constants_to_device();

      lif_spiking_neurons_data_struct temp_neuron_data;
      memcpy(&temp_neuron_data, neuron_data, sizeof(spiking_neurons_data_struct));
      free(neuron_data);
      neuron_data = new lif_spiking_neurons_data_struct();
      memcpy(neuron_data, &temp_neuron_data, sizeof(spiking_neurons_data_struct));
      lif_spiking_neurons_data_struct* this_neuron_data = static_cast<lif_spiking_neurons_data_struct*>(neuron_data);
      
      this_neuron_data->membrane_potentials_v = membrane_potentials_v;
      this_neuron_data->total_number_of_neurons = frontend()->total_number_of_neurons;
      this_neuron_data->thresholds_for_action_potential_spikes = thresholds_for_action_potential_spikes;
      this_neuron_data->resting_potentials_v0 = resting_potentials_v0;
      this_neuron_data->after_spike_reset_potentials_vreset = after_spike_reset_potentials_vreset;
      this_neuron_data->refractory_periods = refractory_periods;
      this_neuron_data->membrane_time_constants_tau_m = membrane_time_constants_tau_m;
      this_neuron_data->membrane_resistances_R = membrane_resistances_R;
      this_neuron_data->background_currents = background_currents;
      
      this_neuron_data->refraction_counter = refraction_counter;
      this_neuron_data->neuron_labels = neuron_labels;

      CudaSafeCall(cudaMemcpy(d_neuron_data,
                              neuron_data,
                              sizeof(lif_spiking_neurons_data_struct),
                              cudaMemcpyHostToDevice));
      
    }

    void LIFSpikingNeurons::reset_state() {
      SpikingNeurons::reset_state();
      vector<int> tmp_refraction_counter;
      for (int i=0; i < frontend()->total_number_of_neurons; i++){
        tmp_refraction_counter.push_back(0);
      }
      CudaSafeCall(cudaMemcpy(refraction_counter,
                              tmp_refraction_counter.data(),
                              frontend()->total_number_of_neurons*sizeof(int),
                              cudaMemcpyHostToDevice));
      CudaSafeCall(cudaMemcpy(membrane_potentials_v,
                              frontend()->membrane_potentials_v.data(),
                              sizeof(float)*frontend()->membrane_potentials_v.size(),
                              cudaMemcpyHostToDevice));
    }

    void LIFSpikingNeurons::state_update(unsigned int current_time_in_timesteps, float timestep, unsigned int timestep_grouping) {
      ::Backend::CUDA::SpikingSynapses* synapses_backend =
        dynamic_cast<::Backend::CUDA::SpikingSynapses*>(frontend()->model->spiking_synapses->backend());
      lif_update_membrane_potentials<<<number_of_neuron_blocks_per_grid, threads_per_block>>>
        (synapses_backend->host_injection_kernel,
         synapses_backend->d_synaptic_data,
         d_neuron_data,
         timestep,
         timestep_grouping,
         current_time_in_timesteps*timestep,
         current_time_in_timesteps,
         frontend()->total_number_of_neurons);

      CudaCheckError();
    }
    /* KERNELS BELOW */
    __global__ void lif_update_membrane_potentials(
        injection_kernel current_injection_kernel,
        spiking_synapses_data_struct* synaptic_data,
        spiking_neurons_data_struct* in_neuron_data,
        float timestep,
        int timestep_grouping,
        float current_time_in_seconds,
        unsigned int current_time_in_timesteps,
        size_t total_number_of_neurons) {
      // Get thread IDs
      int idx = threadIdx.x + blockIdx.x * blockDim.x;
      lif_spiking_neurons_data_struct* neuron_data = (lif_spiking_neurons_data_struct*) in_neuron_data;
      
      if (idx == 0){
        neuron_data->num_activated_neurons[((current_time_in_timesteps / timestep_grouping) + 1) % 2] = 0;
      }
      
      while (idx < total_number_of_neurons) {

        int neuron_label = neuron_data->neuron_labels[idx];
        float mem_time_constant = neuron_data->membrane_time_constants_tau_m[neuron_label];
        float mem_decay_constant = timestep / mem_time_constant;
        float resting_potential_V0 = neuron_data->resting_potentials_v0[neuron_label];
        float temp_membrane_resistance_R = neuron_data->membrane_resistances_R[neuron_label];
        float background_current = neuron_data->background_currents[neuron_label];
        float refractory_period = neuron_data->refractory_periods[neuron_label];
        int refractory_period_in_timesteps = refractory_period / timestep;
        float voltage_input_for_timestep = 0.0f;
        int bufsize = neuron_data->neuron_spike_time_bitbuffer_bytesize[0];
        
        float membrane_potential_Vi = neuron_data->membrane_potentials_v[idx];
      
          
        for (int g=0; g < timestep_grouping; g++){
          int bitloc = (current_time_in_timesteps + g) % (bufsize*8);
          neuron_data->neuron_spike_time_bitbuffer[idx*bufsize + (bitloc / 8)] &= ~(1 << (bitloc % 8));
          #ifndef INLINEDEVICEFUNCS
            voltage_input_for_timestep = current_injection_kernel(
                  synaptic_data,
                  neuron_data,
                  temp_membrane_resistance_R,
                  membrane_potential_Vi,
                  current_time_in_timesteps,
                  timestep,
                  idx,
                  g);
          #else
            switch (synaptic_data->synapse_type)
            {
              case CONDUCTANCE: 
                voltage_input_for_timestep = INLINE_LIF::my_conductance_spiking_injection_kernel(
                  synaptic_data,
                  neuron_data,
                  temp_membrane_resistance_R,
                  membrane_potential_Vi,
                  current_time_in_timesteps,
                  timestep,
                  idx,
                  g);
                break;
              case CURRENT: 
                voltage_input_for_timestep = INLINE_LIF::my_current_spiking_injection_kernel(
                  synaptic_data,
                  neuron_data,
                  temp_membrane_resistance_R,
                  membrane_potential_Vi,
                  current_time_in_timesteps,
                  timestep,
                  idx,
                  g);
                break;
              case VOLTAGE: 
                voltage_input_for_timestep = INLINE_LIF::my_voltage_spiking_injection_kernel(
                  synaptic_data,
                  neuron_data,
                  temp_membrane_resistance_R,
                  membrane_potential_Vi,
                  current_time_in_timesteps,
                  timestep,
                  idx,
                  g);
                break;
              default:
                break;
            }
          #endif
          if (neuron_data->refraction_counter[idx] <= 0){
            membrane_potential_Vi += mem_time_constant*((resting_potential_V0 - membrane_potential_Vi) + background_current)
            membrane_potential_Vi += resting_potential_V0 - equation_constant * membrane_potential_Vi + background_current + voltage_input_for_timestep;
            
    
            // Finally check for a spike
            if (membrane_potential_Vi >= neuron_data->thresholds_for_action_potential_spikes[neuron_label]){
              // Setting the correct bit for the current spike
              neuron_data->neuron_spike_time_bitbuffer[idx*bufsize + (bitloc / 8)] |= (1 << (bitloc % 8));
              
              neuron_data->refraction_counter[idx] = refractory_period_in_timesteps;
              membrane_potential_Vi = neuron_data->after_spike_reset_potentials_vreset[neuron_label];

              // Recording the neuron which has spiked and which sub-timestep within which it did
              int pos = atomicAdd(&neuron_data->num_activated_neurons[(current_time_in_timesteps / timestep_grouping) % 2], 1);
              neuron_data->activated_neuron_ids[pos] = idx;
              neuron_data->activation_subtimesteps[pos] = g;
            }

          } else {
            neuron_data->refraction_counter[idx] -= 1;
          }
      }
      neuron_data->membrane_potentials_v[idx] = membrane_potential_Vi;
      idx += blockDim.x * gridDim.x;
      }
    } 


  } // namespace CUDA
} // namespace Backend
