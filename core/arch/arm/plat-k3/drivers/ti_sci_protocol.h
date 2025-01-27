/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (C) 2016-2022 Texas Instruments Incorporated - https://www.ti.com/
 *	Lokesh Vutla <lokeshvutla@ti.com>
 *	Manorit Chawdhry <m-chawdhry@ti.com>
 */

#ifndef TI_SCI_PROTOCOL_H
#define TI_SCI_PROTOCOL_H

#include <compiler.h>
#include <stdint.h>
#include <util.h>

/* Generic Messages */
#define TI_SCI_MSG_VERSION               0x0002

/* Security Management Messages */
#define TI_SCI_MSG_SA2UL_GET_DKEK        0x9029

/**
 * struct ti_sci_secure_msg_hdr - Secure Message Header for All messages
 *				 and responses
 *
 * @checksum:	Integrity check for HS devices
 * @reserved:	Reserved for future uses
 */
struct ti_sci_secure_msg_hdr {
	uint16_t checksum;
	uint16_t reserved;
} __packed;

/**
 * struct ti_sci_msg_hdr - Generic Message Header for All messages and responses
 * @type:	Type of messages: One of TI_SCI_MSG* values
 * @host:	Host of the message
 * @seq:	Message identifier indicating a transfer sequence
 * @flags:	Flag for the message
 */
struct ti_sci_msg_hdr {
	struct ti_sci_secure_msg_hdr sec_hdr;
	uint16_t type;
	uint8_t host;
	uint8_t seq;
#define TI_SCI_MSG_FLAG(val)			BIT(val)
#define TI_SCI_FLAG_REQ_GENERIC_NORESPONSE	0x0
#define TI_SCI_FLAG_REQ_ACK_ON_RECEIVED		TI_SCI_MSG_FLAG(0)
#define TI_SCI_FLAG_REQ_ACK_ON_PROCESSED	TI_SCI_MSG_FLAG(1)
#define TI_SCI_FLAG_RESP_GENERIC_NACK		0x0
#define TI_SCI_FLAG_RESP_GENERIC_ACK		TI_SCI_MSG_FLAG(1)
	/* Additional Flags */
	uint32_t flags;
} __packed;

/**
 * struct ti_sci_msg_version_req - Request for firmware version information
 * @hdr:	Generic header
 *
 * Request for TI_SCI_MSG_VERSION
 */
struct ti_sci_msg_req_version {
	struct ti_sci_msg_hdr hdr;
} __packed;

/**
 * struct ti_sci_msg_resp_version - Response for firmware version information
 * @hdr:		Generic header
 * @firmware_description: String describing the firmware
 * @firmware_revision:	Firmware revision
 * @abi_major:		Major version of the ABI that firmware supports
 * @abi_minor:		Minor version of the ABI that firmware supports
 * @sub_version:	Sub-version number of the firmware
 * @patch_version:	Patch-version number of the firmware.
 *
 * In general, ABI version changes follow the rule that minor version increments
 * are backward compatible. Major revision changes in ABI may not be
 * backward compatible.
 *
 * Response to request TI_SCI_MSG_VERSION
 */
struct ti_sci_msg_resp_version {
	struct ti_sci_msg_hdr hdr;
#define FIRMWARE_DESCRIPTION_LENGTH 32
	char firmware_description[FIRMWARE_DESCRIPTION_LENGTH];
	uint16_t firmware_revision;
	uint8_t abi_major;
	uint8_t abi_minor;
	uint8_t sub_version;
	uint8_t patch_version;
} __packed;

/**
 * struct ti_sci_msg_sa2ul_get_dkek_req - Request for DKEK value
 * @hdr:			Generic header
 * @sa2ul_instance:		SA2UL instance number - set to 0
 * @kdf_label_len:		Length of "Label" input to KDF
 * @kdf_context_len:		Length of "Context" input to KDF
 * @kdf_label_and_context:	"Label" and "Context" bytes
 *
 * Request for TI_SCI_MSG_SA2UL_GET_DKEK
 */
struct ti_sci_msg_req_sa2ul_get_dkek {
	struct ti_sci_msg_hdr hdr;
	uint8_t sa2ul_instance;
	uint8_t kdf_label_len;
	uint8_t kdf_context_len;
#define KDF_LABEL_AND_CONTEXT_LEN_MAX 41
	uint8_t kdf_label_and_context[KDF_LABEL_AND_CONTEXT_LEN_MAX];
} __packed;

/**
 * struct ti_sci_msg_sa2ul_get_dkek_req - Response for DKEK value
 * @hdr:	Generic header
 * @dkek:	Array containing Derived KEK
 *
 * Response to request TI_SCI_MSG_SA2UL_GET_DKEK
 */
struct ti_sci_msg_resp_sa2ul_get_dkek {
	struct ti_sci_msg_hdr hdr;
#define SA2UL_DKEK_KEY_LEN 32
	uint8_t dkek[SA2UL_DKEK_KEY_LEN];
} __packed;

#endif
