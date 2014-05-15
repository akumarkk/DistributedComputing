#include <stdio.h>

#include "dc_msgdef.h"

//#pragma weak dc_msg_handshake_addme = dc_msg_server_addme_message

msg_table_t	server_msg_table[] = 
{
		{ DC_MSG_HANDSHAKE_ADDME, 	dc_msg_server_addme_message	 },
		{ DC_MSG_HEARTBEAT_MESSAGE, dc_msg_heartbeat_message },
		{ DC_MSG_PROBLEM_REQUEST, 	dc_msg_problem_request	 }
};

typedef struct	dc_msgid_log_
{
	msg_t	msg_id;
	char	message[1024];
}dc_msgid_log_t;

dc_msgid_log_t	dc_msgid_messages[] = 
{
	{ DC_MSG_HANDSHAKE_ADDME, 	"HANDSHAKE MESSAGE - Add me message"},
	{ DC_MSG_HEARTBEAT_MESSAGE, "HEARDT-BEAT MESSAGE - Keep alive message" },
	{ DC_MSG_PROBLEM_REQUEST,	"PRPBLEM_REQUEST_MESSAGE - Problem to be solved"}
};

char *
get_message_from_msgid(msg_t	msgid)
{
	int		n=0, i=0;
	static	char	*msg = "Unknown msgid";

	n = sizeof(dc_msgid_messages)/ sizeof(dc_msgid_log_t);
	for(i = 0; i<n; i++)
	{
		if(dc_msgid_messages[i].msg_id == msgid)
			return dc_msgid_messages[i].message;
	}

	return msg;
}

msg_handler_t
get_msghandler_from_type(msg_t	msg_id)
{
	msg_handler_t	handler = NULL;
	uint16_t		n=0, i=0;

	n = sizeof(server_msg_table) / sizeof(server_msg_table);
    for(i = 0; i<n; i++)
    {
        if(server_msg_table[i].msg_id == msg_id)
		{
            handler = server_msg_table[i].msg_handler;
    		printf("Found handler fot msg_id : %d\n", msg_id);
			break;
		}
	}

	return handler;
}

/*int
dc_msg_server_addme_message(
		connection_t conn,	
        char        *payload,
        uint16_t    payload_len,
        char        **return_payload,
        uint16_t    return_payload_len);
{
    printf("%s: Processing addme message\n", __FUNCTION__);
    return 0;

} 
*/

int
dc_msg_heartbeat_message(
		connection_t conn,	
        char        *payload,
        uint16_t    payload_len,
        char        **return_payload,
        uint16_t    return_payload_len)
{
    printf("%s: Processing addme message\n", __FUNCTION__);
    return 0;

}

int
dc_msg_problem_request(
		connection_t conn,	
        char        *payload,
        uint16_t    payload_len,
        char        **return_payload,
        uint16_t    return_payload_len)
{
    printf("%s: Processing addme message\n", __FUNCTION__);
    return 0;

}

