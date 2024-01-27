#include "ekg/gpu/api.hpp"

void ekg::gpu::api::set_rendering_shader_fragment_source(std::string_view source) {
  this->rendering_shader_fragment_source = source;
}