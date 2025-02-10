#ifndef DIGITAL_FILTER_H_
#define DIGITAL_FILTER_H_

#include <cstdint>

namespace digital_filter {

  class LowPassFilter {
    public:
      LowPassFilter(float rate, float freq, float q);

      float CalcOutput(float value);

    private:
      float sampling_rate_;
      float cutoff_frequency_;
      float filter_q_;
      float omega_;
      float alpha_;

      float in_[3];
      float out_[3];

      float a_[3];
      float b_[3];

      float b0_a0_;
      float b1_a0_;
      float b2_a0_;
      float a1_a0_;
      float a2_a0_;
  };
}

#endif