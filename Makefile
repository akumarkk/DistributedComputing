CC=gcc
CFLAGs=-g
DEPS=dc_server_msgs.h client_info.h dc_connection.h dc_msgdef.h dc_packet.h dc_tlv_defs.h dc_parse_utils.h
#OBJS=dc_client.o dc_msgdef.o dc_packet.o dc_parse_utils.o 
OBJS_SERVER=dc_server.o dc_packet.o dc_parse_utils.o dc_server_msgs.o
%.o: %.c $(DEPS) Makefile
	$(CC) -o $@ -c $< $(CFLAGS)

#dc_clientmain : $(OBJS)
#	$(CC) -o $@ $^ $(CFLAGS)

dc_servermain : $(OBJS_SERVER)
	$(CC) -o $@ $^ $(CFLAGS)
