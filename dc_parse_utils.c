#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdint.h>
#include <ctype.h>

#define FAILURE -1

typedef struct	keyword_
{
	char		key[256];
	uint16_t	opcode;
}keyword_t;

typedef	enum opcode_
{
	sBindAddress,
	sBindport,
	sJoinAddr,
	sJoinPort,
	cDestMulticasteAddr,
	cDestMultiCasteport,
	BadOption
}opcode_t;

keyword_t	keywords[] = 
{
	{ "sServerBindAddress", sBindAddress },
	{ "sServerBindPort", 	sBindport },
	{ "sServerJoinAddress", sJoinAddr },
	{ "sServerJoinPort", sJoinPort },
	{ "cDestMultiCasteAddress", cDestMulticasteAddr },
	{ "cDestMultiCastePort", cDestMultiCasteport }
};

uint16_t
get_opcode_from_keyword(char	*key)
{
	uint16_t	n = sizeof(keywords)/sizeof(keyword_t);
	int			i = 0;
	for(i=0; i<n; i++)
	{
		if(strcasecmp(keywords[i].key, key) == 0)
			return keywords[i].opcode;
	}

	return BadOption;
}
	
int
get_value_of_option(FILE *fp, opcode_t	op, char *get_value)
{
    //FILE    *fp = fopen(CLIENT_CONFIG_FILE, "r");
    int     len = 0;
    char    f_line[1024], *value = NULL, *line=NULL;
	opcode_t	tmp_opc;

    if(fp == NULL || (get_value == NULL))
        return -1;

    while( value=fgets(f_line, sizeof(f_line), fp) )
    {
        //printf("line : %s\n", f_line);
        line = f_line;

        if(value = strchr(line, '='))
        {
            *(value) = '\0';
            value++;
            /* Remove the leading white spaces */
            while( isspace(value[0]) )
                value++;

            /*Remove the trailing white spaces */
            len = strlen(value);
            while( isspace(value[len - 1]) )
                value[--len] = '\0';

            while( isspace(line[0]) )
                line++;
            len = strlen(line);
            while( isspace(line[len-1]) )
                line[--len] = '\0';

            //printf("key = #%s# \tValue = #%s#\n", line, value);
			tmp_opc = get_opcode_from_keyword( line );
			if(tmp_opc = op)
			{
				strcpy(get_value, value);
				get_value[strlen(value) - 1] = '\0';

				return 0;
			}

        }
        memset(line, 0, sizeof(line));
    }

	return -1;
}

