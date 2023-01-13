/*
 * VOKEGPU EKG LICENSE
 *
 * Respect ekg license policy terms, please take a time and read it.
 * 1- Any "skidd" or "stole" is not allowed.
 * 2- Forks and pull requests should follow the license policy terms.
 * 3- For commercial use, do not sell without give credit to vokegpu ekg.
 * 4- For ekg users and users-programmer, we do not care, free to use in anything (utility, hacking, cheat, game, software).
 * 5- Malware, rat and others virus. We do not care.
 * 6- Do not modify this license under any instance.
 *
 * @VokeGpu 2023 all rights reserved.
 */

#ifndef EKG_CORE_FEATURE_H
#define EKG_CORE_FEATURE_H

#include <iostream>
#include <vector>
#include <map>

namespace ekg {
	class feature {};

    template<typename t>
    class feature_manager {
    protected:
        std::map<std::string, std::vector<t>> feature_map {};
    public:
        void insert(const std::string &key) {
            this->feature_map[key] = {};
        }

        std::vector<t> &at(const std::string &key) {
            return this->feature_map[key];
        }

        std::vector<t> &operator[](const std::string &key) {
            return this->feature_map[key];
        }
    };
}

#endif