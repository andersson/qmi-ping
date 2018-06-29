#include <sys/select.h>
#include <sys/time.h>
#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <libqrtr.h>
#include <limits.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "qmi_test.h"

static int last_txn = 1;
static struct timeval last_tv;
bool sigint_received;

static int ping(int fd, unsigned int node, unsigned int port)
{
	struct test_ping_req req = { .ping = "ABC" };
	DEFINE_QRTR_PACKET(req_buf, 256);
	ssize_t len;

	len = qmi_encode_message(&req_buf,
				 QMI_REQUEST, TEST_PING, last_txn++,
				 &req, test_ping_req_ei);
	if (len < 0) {
		fprintf(stderr, "failed to encode ping request: %s\n",
			strerror(-len));
		return -1;
	}

	gettimeofday(&last_tv, NULL);

	return qrtr_sendto(fd, node, port, req_buf.data, req_buf.data_len);
}

static void handle_ping(const struct qrtr_packet *pkt)
{
	struct test_ping_resp resp = {};
	struct timeval delta = {};
	struct timeval now;
	unsigned int txn;
	int ret;

	ret = gettimeofday(&now, NULL);
	if (!ret)
		timersub(&delta, &now, &last_tv);

	ret = qmi_decode_message(&resp, &txn, pkt, QMI_RESPONSE,
				 TEST_PING, test_ping_resp_ei);
	if (ret < 0) {
		fprintf(stderr, "unable to aprse response\n");
		return;
	}

	printf("%zd bytes from %d:%d: txn=%d time=%ld.%ld\n", pkt->data_len, pkt->node, pkt->port, txn, (long)delta.tv_sec, (long)delta.tv_usec);
}

static void usage(void)
{
	extern const char *__progname;

	fprintf(stderr, "usage: %s node:port\n", __progname);
	exit(1);
}

static void sigint(int sig)
{
	sigint_received = true;
}

int main(int argc, char **argv)
{
	struct sockaddr_qrtr sq;
	struct qrtr_packet pkt;
	unsigned int msg_id;
	unsigned long node;
	unsigned long port = 0;
	struct timeval tv = { INT_MAX, };
	unsigned int received = 0;
	unsigned int count = 0;
	unsigned int sent = 0;
	socklen_t sl;
	char buf[4096];
	fd_set fds;
	int opt;
	int ret;
	int fd;

	while ((opt = getopt(argc, argv, "c:")) != -1) {
		switch (opt) {
		case 'c':
			count = strtoul(optarg, NULL, 10);
			break;
		default:
			usage();
		}
	}

	if (optind >= argc)
		usage();

	node = strtoul(argv[optind], NULL, 10);

	signal(SIGINT, sigint);

	fd = qrtr_open(0);
	if (fd < 0)
		err(1, "failed to create qrtr socket");

	fcntl(fd, F_SETFL, O_NONBLOCK);

	ret = qrtr_new_lookup(fd, 15, 1, 0);
	if (ret < 0)
		err(1, "failed to register new lookup");

	while (!sigint_received) {
		FD_ZERO(&fds);
		FD_SET(fd, &fds);

		ret = select(fd + 1, &fds, NULL, NULL, &tv);
		if (!ret) {
			if (port) {
				if (count && sent == count)
					break;
				
				ping(fd, node, port);

				sent++;

				tv.tv_sec = 1;
				tv.tv_usec = 0;
			} else {
				tv.tv_sec = INT_MAX;
				tv.tv_usec = 0;
			}
			continue;
		}

		if (ret < 0 || !FD_ISSET(fd, &fds))
			continue;

		sl = sizeof(sq);
		ret = recvfrom(fd, buf, sizeof(buf), 0, (void *)&sq, &sl);
		if (ret < 0) {
			if (errno == EAGAIN)
				continue;
			return -1;
		}

		ret = qrtr_decode(&pkt, buf, ret, &sq);
		if (ret < 0) {
			errno = -ret;	
			err(1, "failed to decode incoming message");
		}

		switch (pkt.type) {
		case QRTR_TYPE_NEW_SERVER:
			if (!pkt.node && !pkt.port)
				break;

			if (pkt.node == node) {
				port = pkt.port;

				tv.tv_sec = 0;
				tv.tv_usec = 0;

				printf("ping %ld:%ld\n", node, port);
			}
			break;
		case QRTR_TYPE_DEL_SERVER:
			if (pkt.node == node && pkt.port == port) {
				port = 0;
				printf("del: %d %d\n", pkt.node, pkt.port);
			}
			break;
		case QRTR_TYPE_DATA:
			ret = qmi_decode_header(&pkt, &msg_id);
			if (ret < 0)
				return ret;

			switch (msg_id) {
			case TEST_PING:
				handle_ping(&pkt);
				received++;

				if (sent == count)
					goto done;
				break;
			default:
				fprintf(stderr, "[RMTFS] Unknown request: %d\n", msg_id);
				break;
			}
		}
	}

done:

	printf("%d packets transmitted, %d received %d%% packet loss\n", sent, received, 100 * (sent - received) / sent);

	return 0;
}
