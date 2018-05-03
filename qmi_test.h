#ifndef __QMI_TEST_H__
#define __QMI_TEST_H__

#include <stdint.h>
#include <stdbool.h>

#include "libqrtr.h"

#define TEST_PING 32

struct test_qmi_result {
	uint16_t result;
	uint16_t error;
};

struct test_name {
};

struct test_ping_req {
	uint32_t ping_len;
	uint8_t ping[4];
	bool client_valid;
	struct test_name client;
};

struct test_ping_resp {
	struct test_qmi_result result;
	bool pong_valid;
	uint32_t pong_len;
	uint8_t pong[4];
	bool client_valid;
	struct test_name client;
};

extern struct qmi_elem_info test_ping_req_ei[];
extern struct qmi_elem_info test_ping_resp_ei[];

#endif
