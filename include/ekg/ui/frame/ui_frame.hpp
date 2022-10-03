#ifndef EKG_UI_FRAME_H
#define EKG_UI_FRAME_H

#include "ekg/ui/abstract/ui_abstract.hpp"
#include "ekg/util/geometry.hpp"
#include <vector>

namespace ekg::ui {
class frame : public ekg::ui::abstract {
	protected:
		std::vector<uint32_t> parent_id_list {};

		ekg::dock dock_resize {};
		ekg::dock dock_drag {};
		ekg::dock dock_embed {};

		ekg::vec2 initial_pos {};
		ekg::vec2 initial_size {};
		ekg::vec2 pos {};
		ekg::vec2 size {};
	public:
		void set_drag(const ekg::dock &dock);
		ekg::dock get_drag_dock();

		void set_resize(const ekg::dock &dock);
		ekg::dock get_resize_dock();

		void set_pos_initial(float x, float y);
		ekg::vec2 get_pos_initial();

		void set_size_initial(float w, float h);
		ekg::vec2 get_size_initial();

		void parent(uint32_t token);
		std::vector<uint32_t> &get_parent_id_list();

		void set_embed(const ekg::dock &dock);
		ekg::dock get_embed_dock();

		void set_size(float w, float h);
		ekg::vec2 get_size();

		void set_pos(float x, float y);
		ekg::vec2 get_pos();
	};
}

#endif