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
#include "config.h"

#include "server.hpp"
#include <boost/bind.hpp>

namespace http {
namespace server {

server::server(const std::string& address, const std::string& port,
		const std::string& doc_root, std::size_t io_service_pool_size) :
		io_service_pool_(io_service_pool_size), signals_(
				io_service_pool_.get_io_service()), acceptor_(
				io_service_pool_.get_io_service()), new_connection_(), request_handler_(
				doc_root) {
	signals_.add(SIGINT);
	signals_.add(SIGTERM);
#if defined(SIGQUIT)
	signals_.add(SIGQUIT);
#endif // defined(SIGQUIT)
	signals_.async_wait(boost::bind(&server::handle_stop, this));

	boost::asio::ip::tcp::resolver resolver(acceptor_.get_io_service());
	boost::asio::ip::tcp::resolver::query query(address, port);
	boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
	acceptor_.open(endpoint.protocol());
	acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
	acceptor_.bind(endpoint);
	acceptor_.listen();

	start_accept();
}

void server::run() {
	io_service_pool_.run();
}

void server::start_accept() {
	new_connection_.reset(
			new connection(io_service_pool_.get_io_service(),
					request_handler_));
	acceptor_.async_accept(new_connection_->socket(),
			boost::bind(&server::handle_accept, this,
					boost::asio::placeholders::error));
}

void server::handle_accept(const boost::system::error_code& e) {
	if (!e) {
		new_connection_->start();
	}

	start_accept();
}

void server::handle_stop() {
	io_service_pool_.stop();
}

} // namespace server
} // namespace http
