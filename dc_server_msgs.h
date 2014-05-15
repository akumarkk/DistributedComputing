#ifndef _DC_SERVER_MSGS_H_
#define _DC_SERVER_MSGS_H_

int
dc_msg_server_addme_message(connection_t, void *payload, uint16_t payload_len, void **return_payload, uint16_t ret_payload_len);


int
dc_msg_handshake_addme(connection_t, void *payload, uint16_t payload_len, void **return_payload, uint16_t ret_payload_len);

int
dc_msg_heartbeat_message(connection_t, void *payload, uint16_t payload_len, void **return_payload, uint16_t ret_payload_len);

int
dc_msg_problem_request(connection_t, void *payload, uint16_t payload_len, void **return_payload, uint16_t ret_payload_len);

#endif /* _DC_SERVER_MSGS_H_ */
