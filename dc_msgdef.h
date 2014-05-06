#ifndef _DC_MSGDEF_H_
#define _DC_MSGDEF_H_

/* Definition of Message types used in communication between Client and Server */
enum msg_
{
	DC_MSG_HANDSHAKE_ADDME,
	DC_MSG_HEARTBEAT_GET,
	DC_MSG_PROBLEM_REQUEST
}msg_t;

/* Structure to hold the msg_t and corresponding handler info */

typedef	struct	msg_table_
{
	msg_t	msg_id;
	int 	(*msg_handler)(char *, uint16_t , char **, uint16_t);
}msg_table_t;
