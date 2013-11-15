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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <signal.h>
#include <limits.h>
#include <fcntl.h>
#include <assert.h>

#include <atomic>
#include <memory>

#include "blocking-queue.h"

typedef struct {
	int core;
#ifdef PTHREAD_IPC
	BlockingQueue<int> queue;
#else
	int pipefd;
#endif
	std::vector<pthread_t> clients;
} handler_args;

typedef struct {
	int core;
	int sock;
	int client_threads;
} acceptor_args;

const static char* ok_response = "HTTP/1.0 200 OK\n"
		"Content-type: text/html\n"
		"\n";

const static char* bad_request_response = "HTTP/1.0 400 Bad Request\n"
		"Content-type: text/html\n"
		"\n"
		"<html>\n"
		" <body>\n"
		"  <h1>Bad Request</h1>\n"
		"  <p>This server did not understand your request.</p>\n"
		" </body>\n"
		"</html>\n";

const static char* bad_method_response_template =
		"HTTP/1.0 501 Method Not Implemented\n"
				"Content-type: text/html\n"
				"\n"
				"<html>\n"
				" <body>\n"
				"  <h1>Method Not Implemented</h1>\n"
				"  <p>The method %s is not implemented by this server.</p>\n"
				" </body>\n"
				"</html>\n";

std::vector<pthread_t> acceptors;
std::atomic<bool> guru_meditation(false);
static int cleanup_pop_arg = 0;

#ifdef USE_AFFINITY
int setaffinity(int core)
{
	int cores = sysconf(_SC_NPROCESSORS_ONLN);
	if (core >= cores)
	return EINVAL;

	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(core, &cpuset);

	pthread_t current_thread = pthread_self();
	return pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset);
}
#endif

void* sig_thread(void* arg) {
	sigset_t *set = (sigset_t *) arg;
	int sig;

	if (sigwait(set, &sig) != 0) {
		perror("sigwait");
		exit (EXIT_FAILURE);
	}

	printf("\ncaught signal %d, guru meditation...\n", sig);
	guru_meditation = true;

	void* res;
	for (std::vector<pthread_t>::iterator it = acceptors.begin();
			it != acceptors.end(); ++it) {
		pthread_cancel(*it);
		pthread_join(*it, &res);
	}

	return NULL;
}

void cleanup_handler(void *arg) {
	handler_args* h_args = (handler_args*) arg;
	assert(h_args);

#ifdef PTHREAD_IPC
	h_args->queue.interrupt();
#else
	for (std::vector<pthread_t>::iterator it = h_args->clients.begin(); it != h_args->clients.end(); ++it)
	{
		pthread_cancel(*it);
	}
#endif
}

void* client_handler(void* arg) {
	handler_args* h_args = (handler_args*) arg;
	assert(h_args);
#ifdef USE_AFFINITY
	setaffinity(h_args->core);
#endif

	int connection_fd = 0;

	while (1) {
#ifdef PTHREAD_IPC
		try {
			connection_fd = h_args->queue.pop();
		} catch (const InterruptedException& e) {
			assert(guru_meditation);
			return NULL;
		}
#else
		read(h_args->pipefd, &connection_fd, sizeof(connection_fd));
#endif

		assert(connection_fd > 0);

		char buffer[1024 * 2];
		ssize_t bytes_read;

		bytes_read = read(connection_fd, buffer, sizeof(buffer) - 1);
		if (bytes_read > 0) {
			char method[sizeof(buffer)];
			char url[sizeof(buffer)];
			char protocol[sizeof(buffer)];

			buffer[bytes_read] = '\0';
			sscanf(buffer, "%s %s %s", method, url, protocol);

			while (strstr(buffer, "\r\n\r\n") == NULL)
				bytes_read = read(connection_fd, buffer, sizeof(buffer));

			if (bytes_read == -1) {
				close(connection_fd);
				continue;
			}

			if (strcmp(protocol, "HTTP/1.0") && strcmp(protocol, "HTTP/1.1")) {
				size_t sent = write(connection_fd, bad_request_response,
						strlen(bad_request_response));
				assert(sent == strlen(bad_request_response));
			} else if (strcmp(method, "GET")) {
				char response[1024];

				snprintf(response, sizeof(response),
						bad_method_response_template, method);
				size_t sent = write(connection_fd, response, strlen(response));
				assert(sent == strlen(response));
			} else {
				int fd = open(url, O_RDONLY);
				if (fd < 0) {
					perror("open");
					size_t sent = write(connection_fd, bad_request_response,
							strlen(bad_request_response));
					assert(sent == strlen(bad_request_response));
				} else {
					struct stat sb;
					if (stat(url, &sb) == -1) {
						perror("stat");
						size_t sent = write(connection_fd, bad_request_response,
								strlen(bad_request_response));
						assert(sent == strlen(bad_request_response));
					} else {
						write(connection_fd, ok_response, strlen(ok_response));
						size_t sent = sendfile(connection_fd, fd, NULL,
								sb.st_size);
						assert(sent == size_t(sb.st_size));
					}
				}

				if (fd > 0)
					close(fd);
			}
		} else if (bytes_read == 0)
			;
		else {
			perror("read");
		}

		close(connection_fd);
	}

	return NULL;
}

void* acceptor(void* arg) {
	acceptor_args* a_args = (acceptor_args*) arg;
	assert(a_args);
#ifdef USE_AFFINITY
	setaffinity(a_args->core);
#endif

#ifndef PTHREAD_IPC
	int pipefd[2];

	if (pipe(pipefd) == -1)
	{
		perror("pipe");
		exit(EXIT_FAILURE);
	}
#endif

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN * 2);

	handler_args h_args;
	h_args.core = a_args->core;

#ifndef PTHREAD_IPC
	h_args.pipefd = pipefd[0];
#endif

	pthread_cleanup_push(cleanup_handler, &h_args);

	for (int i = 0; i < a_args->client_threads; ++i) {
		pthread_t t;
		if (pthread_create(&t, &attr, client_handler, (void*) &h_args) < 0) {
			perror("pthread_create");
			exit (EXIT_FAILURE);
		}

		h_args.clients.push_back(t);
	}

	printf("acceptor %d is ready\n", a_args->core);

	struct addrinfo client_addr;
	socklen_t addr_len = sizeof(client_addr);

	while (1) {
		int connection = accept(a_args->sock, (struct sockaddr*) &client_addr,
				&addr_len);

		if (connection < 0) {
			perror("accept");
			continue;
		}

#ifdef PTHREAD_IPC
		h_args.queue.push(connection);
#else
		write(pipefd[1], &connection, sizeof(connection));
#endif
	}

	pthread_cleanup_pop(cleanup_pop_arg);

	return NULL;
}

int main(int argc, char* argv[]) {
	if (argc != 3) {
		fprintf(stderr, "Usage: %s port clients-per-core\n", argv[0]);
		exit (EXIT_FAILURE);
	}

	pthread_t thread;
	sigset_t set;

	sigemptyset(&set);
	sigaddset(&set, SIGINT);

	if (pthread_sigmask(SIG_BLOCK, &set, NULL) < 0) {
		perror("pthread_sigmask");
		exit (EXIT_FAILURE);
	}

	if (pthread_create(&thread, NULL, &sig_thread, (void*) &set) < 0) {
		perror("pthread_create");
		exit (EXIT_FAILURE);
	}

	signal(SIGPIPE, SIG_IGN);

	int client_threads = atoi(argv[2]);

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN * 2);

	int sockfd;
	struct addrinfo hints;
	struct addrinfo* servinfo;
	struct addrinfo* tmpinfo;
	int reuse = 1;
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if ((rv = getaddrinfo(NULL, argv[1], &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		exit (EXIT_FAILURE);
	}

	for (tmpinfo = servinfo; tmpinfo != NULL; tmpinfo = tmpinfo->ai_next) {
		if ((sockfd = socket(tmpinfo->ai_family, tmpinfo->ai_socktype,
				tmpinfo->ai_protocol)) == -1) {
			perror("socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int))
				== -1) {
			perror("setsockopt");
			exit (EXIT_FAILURE);
		}

		if (bind(sockfd, tmpinfo->ai_addr, tmpinfo->ai_addrlen) == -1) {
			close(sockfd);
			perror("bind");
			continue;
		}

		break;
	}

	if (tmpinfo == NULL) {
		fprintf(stderr, "failed to bind\n");
		exit (EXIT_FAILURE);
	}

	freeaddrinfo(servinfo);

	if (listen(sockfd, SOMAXCONN) == -1) {
		perror("listen");
		exit (EXIT_FAILURE);
	}

	int cores = sysconf(_SC_NPROCESSORS_ONLN);
	acceptor_args args[cores];

	for (int i = 0; i < cores; ++i) {
		args[i].core = i;
		args[i].sock = sockfd;
		args[i].client_threads = client_threads;

		pthread_t t;
		if (pthread_create(&t, &attr, acceptor, (void*) &args[i]) < 0) {
			perror("pthread_create");
			exit (EXIT_FAILURE);
		}

		acceptors.push_back(t);
	}

	void* res;
	for (std::vector<pthread_t>::iterator it = acceptors.begin();
			it != acceptors.end(); ++it) {
		pthread_join(*it, &res);
	}

	close(sockfd);

	return 0;
}
