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
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include "../server.hpp"

int main(int argc, char* argv[]) {
	try {
		if (argc != 5) {
			std::cerr
					<< "Usage: http_server <address> <port> <threads> <doc_root>\n";
			std::cerr << "  For IPv4, try:\n";
			std::cerr << "    receiver 0.0.0.0 80 1 .\n";
			std::cerr << "  For IPv6, try:\n";
			std::cerr << "    receiver 0::0 80 1 .\n";
			return 1;
		}

		std::size_t num_threads = boost::lexical_cast < std::size_t > (argv[3]);
		http::server::server s(argv[1], argv[2], argv[4], num_threads);

		s.run();
	} catch (std::exception& e) {
		std::cerr << "exception: " << e.what() << "\n";
	}

	return 0;
}
