CC=gcc
CFLAGs=-g -I.
DEPS=client_info.h dc_connection.h dc_msgdef.h dc_packet.h dc_tlv_defs.h
OBJS=dc_client.o dc_msgdef.o dc_packet.o dc_parse_utils.o 

%.o: %.c $(DEPS)
	$(CC) -o $@ -c $< $(CFLAGS)

dc_clientmain : $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)
