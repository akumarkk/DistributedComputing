#include "dc_msgdef.h"

msg_table_t	server_msg_table[]
{
		{ DC_MSG_HANDSHAKE_ADDME, 	dc_msg_handshake_addme	},
		{ DC_MSG_HEARTBEAT_GET, 	dc_msg_heartbeat_get	},
		{ DC_MSG_PROBLEM_REQUEST, 	dc_msg_problem_request	}
};
