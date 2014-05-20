#ifndef _DC_AVL_DEFS_H_
#define _DC_AVL_DEFS_H_

#define DUMP_AVP

typedef enum attr_t_
{
	DC_ATTR_MACHINE_IP,
	DC_ATTR_MULTICAST_GROUP
}attr_t;

typedef struct	avl_
{
	attr_t		attr_id;
	uint16_t	value_len;
	char		value[0];
}avl_t;

#endif /*_DC_AVL_DEFS_H_ */
