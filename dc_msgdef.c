#include <stdio.h>

#include "dc_msgdef.h"

msg_table_t	server_msg_table[] = 
{
		{ DC_MSG_HANDSHAKE_ADDME, 	dc_msg_handshake_addme	},
		{ DC_MSG_HEARTBEAT_GET, 	dc_msg_heartbeat_get	},
		{ DC_MSG_PROBLEM_REQUEST, 	dc_msg_problem_request	}
};

msg_handler_t
get_msghandler_from_type(msg_t	msg_id)
{
	msg_handler_t	handler = NULL;
	uint16_t		n=0, i=0;

	n = sizeof(server_msg_table) / sizeof(server_msg_table);
    for(i = 0; i<n; i++)
    {
        if(server_msg_table[i].msg_id = msg_id)
            handler = server_msg_table[i].msg_handler;
    }

	return handler;
}

int
dc_msg_handshake_addme(
        char        *payload,
        uint16_t    payload_len,
        char        **return_payload,
        uint16_t    return_payload_len)
{
    printf("%s: Processing addme message\n", __FUNCTION__);
    return 0;

}

int
dc_msg_heartbeat_get(
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
        char        *payload,
        uint16_t    payload_len,
        char        **return_payload,
        uint16_t    return_payload_len)
{
    printf("%s: Processing addme message\n", __FUNCTION__);
    return 0;

}

