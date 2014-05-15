#ifndef _DC_MSGDEF_H_
#define _DC_MSGDEF_H_


#include <stdint.h>
#include "dc_connection.h"

#define DUMP
/* Definition of Message types used in communication between Client and Server */
typedef	enum msg_
{
	DC_MSG_HANDSHAKE_ADDME,
	DC_MSG_SERVER_PUBLISH_AVAILABLE_GROUPS,
	DC_MSG_HEARTBEAT_MESSAGE,
	DC_MSG_PROBLEM_REQUEST
}msg_t;

typedef	int (*msg_handler_t)(char *, uint16_t , char **, uint16_t);
/* Structure to hold the msg_t and corresponding handler info */

typedef	struct	msg_table_
{
	msg_t	msg_id;
	int 	(*msg_handler)(connection_t , char *, uint16_t , char **, uint16_t);
}msg_table_t;

/* DC main message type.
 * This is the message exchanged between server and retainers
 */
typedef	struct	dc_msg_
{
	msg_t		msg_type;
	uint32_t	payload_len;
	char 		payload[0];
}dc_msg_t;

int
dc_msg_handshake_addme(connection_t, char *payload, uint16_t payload_len, char **return_payload, uint16_t ret_payload_len);

int
dc_msg_heartbeat_message(connection_t, char *payload, uint16_t payload_len, char **return_payload, uint16_t ret_payload_len);

int
dc_msg_problem_request(connection_t, char *payload, uint16_t payload_len, char **return_payload, uint16_t ret_payload_len);
#endif /* _DC_MSGDEF_H_ */
