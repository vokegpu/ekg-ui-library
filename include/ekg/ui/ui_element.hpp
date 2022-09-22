#pragma once
#ifndef EKG_UI_ELEMENT_H
#define EKG_UI_ELEMENT_H

#include <iostream>
#include <string>

namespace ekg {
	class element {
	protected:
		uint32_t id {};
		uint32_t parent_id {};

		std::string tag {};
		std::string extra_tag {};
	public:
		void set_id(uint32_t token);
		uint32_t get_id();

		void set_parent_id(uint32_t token);
		uint32_t get_parent_id();

		void set_tag(const std::string &str);
		std::string get_tag();

		void set_extra_tag(const std::string &str);
		std::string get_extra_tag();
	};
};

#endif