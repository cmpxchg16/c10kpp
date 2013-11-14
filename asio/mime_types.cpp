/*
    C10KPP - Proof of concept 10K concurrent clients
    Copyright (C) 2013 Uri Shamay and Shachar Shemesh

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

 */
#include "mime_types.hpp"

namespace http {
namespace server {
namespace mime_types {

struct mapping {
	const char* extension;
	const char* mime_type;
} mappings[] =
		{ { "gif", "image/gif" }, { "htm", "text/html" },
				{ "html", "text/html" }, { "jpg", "image/jpeg" }, { "png",
						"image/png" }, { 0, 0 } };

std::string extension_to_type(const std::string& extension) {
	for (mapping* m = mappings; m->extension; ++m) {
		if (m->extension == extension) {
			return m->mime_type;
		}
	}

	return "text/plain";
}

} // namespace mime_types
} // namespace server
} // namespace http
