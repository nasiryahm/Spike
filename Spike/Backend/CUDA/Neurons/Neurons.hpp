#pragma once

#include "Spike/Neurons/Neurons.hpp"
#include "Spike/Backend/CUDA/CUDABackend.hpp"
#include <cuda.h>
#include <vector_types.h>

namespace Backend {
  namespace CUDA {
    struct neurons_data_struct {
      int total_number_of_neurons;
      int * per_neuron_efferent_synapse_count = nullptr;
      int * per_neuron_efferent_synapse_start = nullptr;
    };

    class Neurons : public virtual ::Backend::Neurons {
    public:
      ~Neurons() override;

      void prepare() override;
      void reset_state() override;

      // Device Pointers
      int * per_neuron_afferent_synapse_count = nullptr;  /**< A (device-side) count of the number of afferent synapses for each neuron */
      
      int * per_neuron_efferent_synapse_count = nullptr;
      int * per_neuron_efferent_synapse_start = nullptr;

      dim3 number_of_neuron_blocks_per_grid;    /**< CUDA Device number of blocks */
      dim3 threads_per_block;           /**< CUDA Device number of threads */
      neurons_data_struct* neuron_data;
      neurons_data_struct* d_neuron_data;

      void allocate_device_pointers(); // Not virtual
  
      /**  
       *  Allows copying of static data related to neuron dynamics to the device.
       */
      void copy_constants_to_device(); // Not virtual

      /**  
       *  A local, non-polymorphic function called in to determine the CUDA Device thread (Neurons::threads_per_block) and block dimensions (Neurons::number_of_neuron_blocks_per_grid).
       */
      void set_threads_per_block_and_blocks_per_grid(int threads); // Not virtual

    };
  } // namespace CUDA
} // namespace Backend

