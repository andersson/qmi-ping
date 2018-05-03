#include <errno.h>
#include <string.h>
#include "qmi_test.h"

struct qmi_elem_info test_qmi_result_ei[] = {
	{
		.data_type = QMI_UNSIGNED_2_BYTE,
		.elem_len = 1,
		.elem_size = sizeof(uint16_t),
		.offset = offsetof(struct test_qmi_result, result),
	},
	{
		.data_type = QMI_UNSIGNED_2_BYTE,
		.elem_len = 1,
		.elem_size = sizeof(uint16_t),
		.offset = offsetof(struct test_qmi_result, error),
	},
	{}
};

struct qmi_elem_info test_name_ei[] = {
	{}
};

struct qmi_elem_info test_ping_req_ei[] = {
	{
		.data_type = QMI_UNSIGNED_1_BYTE,
		.elem_len = 4,
		.elem_size = sizeof(uint8_t),
		.array_type = STATIC_ARRAY,
		.tlv_type = 1,
		.offset = offsetof(struct test_ping_req, ping),
	},
	{
		.data_type = QMI_OPT_FLAG,
		.elem_len = 1,
		.elem_size = sizeof(bool),
		.tlv_type = 16,
		.offset = offsetof(struct test_ping_req, client_valid),
	},
	{
		.data_type = QMI_STRUCT,
		.elem_len = 1,
		.elem_size = sizeof(struct test_name),
		.tlv_type = 16,
		.offset = offsetof(struct test_ping_req, client),
		.ei_array = test_name_ei,
	},
	{}
};

struct qmi_elem_info test_ping_resp_ei[] = {
	{
		.data_type = QMI_STRUCT,
		.elem_len = 1,
		.elem_size = sizeof(struct test_qmi_result),
		.tlv_type = 2,
		.offset = offsetof(struct test_ping_resp, result),
		.ei_array = test_qmi_result_ei,
	},
	{
		.data_type = QMI_OPT_FLAG,
		.elem_len = 1,
		.elem_size = sizeof(bool),
		.tlv_type = 16,
		.offset = offsetof(struct test_ping_resp, pong_valid),
	},
	{
		.data_type = QMI_UNSIGNED_1_BYTE,
		.elem_len = 4,
		.elem_size = sizeof(uint8_t),
		.array_type = STATIC_ARRAY,
		.tlv_type = 16,
		.offset = offsetof(struct test_ping_resp, pong),
	},
	{
		.data_type = QMI_OPT_FLAG,
		.elem_len = 1,
		.elem_size = sizeof(bool),
		.tlv_type = 17,
		.offset = offsetof(struct test_ping_resp, client_valid),
	},
	{
		.data_type = QMI_STRUCT,
		.elem_len = 1,
		.elem_size = sizeof(struct test_name),
		.tlv_type = 17,
		.offset = offsetof(struct test_ping_resp, client),
		.ei_array = test_name_ei,
	},
	{}
};

