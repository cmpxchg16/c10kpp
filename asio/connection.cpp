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

#include <iostream>
#include <fstream>
#include <vector>

#include <boost/bind.hpp>

#include "connection.hpp"
#include "request_handler.hpp"

namespace http {
namespace server {

connection::connection(boost::asio::io_service& io_service,
		request_handler& handler) :
		socket_(io_service), request_handler_(handler) {
}

boost::asio::ip::tcp::socket& connection::socket() {
	return socket_;
}

void connection::start() {
	socket_.async_read_some(boost::asio::buffer(buffer_),
			boost::bind(&connection::handle_read, shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
}

void connection::start_file_read() {
	file_.open(full_file_path_.c_str(),
			std::ios_base::binary | std::ios_base::ate);

	if (!file_) {
		reply_ = reply::stock_reply(reply::not_found);
		return;
	}

	file_.seekg(0);

	boost::system::error_code ignored_ec;
	handle_write_file(ignored_ec);
}

void connection::handle_write_file(const boost::system::error_code& err) {
	if (!err) {
		if (file_.eof() == false) {
			file_.read(file_buffer_.c_array(),
					(std::streamsize) file_buffer_.size());
			if (file_.gcount() <= 0) {
				close();
				return;
			}

			boost::asio::async_write(socket_,
					boost::asio::buffer(file_buffer_.c_array(), file_.gcount()),
					boost::bind(&connection::handle_write_file,
							shared_from_this(),
							boost::asio::placeholders::error));
			if (err) {
				std::cout << "send error:" << err << std::endl;
				close();
				return;
			}
		} else
			close();
	} else {
		std::cout << "Error: " << err.message() << "\n";
	}
}

void connection::close() {
	boost::system::error_code ignored_ec;
	socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
	//socket_.close(ignored_ec);
}

void connection::handle_read(const boost::system::error_code& e,
		std::size_t bytes_transferred) {
	if (!e) {
		boost::tribool result;
		boost::tie(result, boost::tuples::ignore) = request_parser_.parse(
				request_, buffer_.data(), buffer_.data() + bytes_transferred);

		if (result) {
			request_handler_.handle_request(request_, reply_, full_file_path_);

			boost::asio::async_write(socket_, reply_.to_buffers(),
					boost::bind(&connection::start_file_read,
							shared_from_this()));
		} else if (!result) {
			reply_ = reply::stock_reply(reply::bad_request);
			boost::asio::async_write(socket_, reply_.to_buffers(),
					boost::bind(&connection::handle_write, shared_from_this(),
							boost::asio::placeholders::error));
		} else {
			socket_.async_read_some(boost::asio::buffer(buffer_),
					boost::bind(&connection::handle_read, shared_from_this(),
							boost::asio::placeholders::error,
							boost::asio::placeholders::bytes_transferred));
		}
	}
}

void connection::handle_write(const boost::system::error_code& e) {
	if (!e) {
		boost::system::error_code ignored_ec;
		socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both,
				ignored_ec);
	}
}

} // namespace server
} // namespace http
