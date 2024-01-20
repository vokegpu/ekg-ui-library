#ifndef EKG_GPU_BASE_IMPL_H
#define EKG_GPU_BASE_IMPL_H

namespace ekg {
  enum class gpu_api {
    vulkan, opengl
  };

  class gpu_base_impl {
  public:
    virtual void init() {};
    virtual void quit() {};
    virtual void invoke_pipeline() {};
    virtual void revoke_pipeline() {};
    virtual void update_viewport(int32_t w, int32_t h) {};
  }
}

#endif