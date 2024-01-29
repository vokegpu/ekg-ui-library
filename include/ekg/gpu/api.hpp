#ifndef EKG_GPU_BASE_IMPL_H
#define EKG_GPU_BASE_IMPL_H

#include <iostream>
#include <string_view>

namespace ekg {
  enum class api {
    vulkan, opengl
  };

  namespace gpu {
    class api {
    protected:
      std::string_view rendering_shader_fragment_source {};
    public:
      void set_rendering_shader_fragment_source(std::string_view source);
    public:
      virtual void init() {};
      virtual void quit() {};
      virtual void invoke_pipeline() {};
      virtual void revoke_pipeline() {};
      virtual void update_viewport(int32_t w, int32_t h) {};
      virtual uint32_t dispatch_sampler(const ekg::sampler_create_info *p_sampler_create_info) { return 0 };
      virtual void draw() {};
      virtual void bind_texture(uint32_t texture_id) {};
    }
  }
}

#endif