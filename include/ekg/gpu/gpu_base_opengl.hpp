#ifndef EKG_GPU_BASE_OPENGL_H
#define EKG_GPU_BASE_OPENGL_H

#include "gpu_base_impl.hpp"

namespace ekg {
  class gpu_base_opengl : public ekg::gpu_base_impl {
  public:
    void init() override;
    void quit() override;

    void invoke_pipeline() override;
    void revoke_pipeline() override;
    void update_viewport(int32_t w, int32_t h) override;
  }
}

#endif