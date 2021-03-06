#pragma once

#include "Spike/Neurons/SpikingNeurons.hpp"
#include "Neurons.hpp"

namespace Backend {
  namespace Dummy {
    class SpikingNeurons : public virtual ::Backend::Dummy::Neurons,
                           public virtual ::Backend::SpikingNeurons {
    public:
      SPIKE_MAKE_BACKEND_CONSTRUCTOR(SpikingNeurons);
      SpikingNeurons();
      void prepare() override;
      void reset_state() override;

      void state_update(unsigned int current_time_in_timesteps,
                                   float timestep) override;
    };
  } // namespace Dummy
} // namespace Backend
