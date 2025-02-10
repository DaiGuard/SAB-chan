#include "digital_filter.h"
#include <cmath>


namespace digital_filter {
  LowPassFilter::LowPassFilter(float rate, float freq, float q) {

    sampling_rate_ = rate;
    cutoff_frequency_ = freq;
    filter_q_ = q;

    omega_ = 2.0f * M_PI * cutoff_frequency_ / sampling_rate_;
    alpha_ = std::sin(omega_) / (2.0f * filter_q_);

    a_[0] = 1.0f + alpha_;
    a_[1] = -2.0f * std::cos(omega_);
    a_[2] = 1.0f - alpha_;

    b_[0] = (1.0f - std::cos(omega_)) / 2.0f;
    b_[1] = 1.0f - std::cos(omega_);
    b_[2] = (1.0f - std::cos(omega_)) / 2.0f;

    b0_a0_ = b_[0] / a_[0];
    b1_a0_ = b_[1] / a_[0];
    b2_a0_ = b_[2] / a_[0];
    a1_a0_ = a_[1] / a_[0];
    a2_a0_ = a_[2] / a_[0];

    for(int i=0; i<3; i++) {
      in_[i] = 0.0f;
      out_[i] = 0.0f;
    }
  }


  float LowPassFilter::CalcOutput(float value) {
    // 値を後退させる
    in_[2] = in_[1];
    in_[1] = in_[0];
    in_[0] = value;

    out_[2] = out_[1];
    out_[1] = out_[0];

    out_[0] = b0_a0_ * in_[0] 
            + b1_a0_ * in_[1] 
            + b2_a0_ * in_[2]
            - a1_a0_ * out_[1]
            - a2_a0_ * out_[2];

    return out_[0];
  }
}