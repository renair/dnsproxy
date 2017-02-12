#include<stdio.h>
#include<string.h>
#include<errno.h>

#include "configurator.h"

#define BUFFSIZE 126

void load_config(char* filename, struct config* conf)
{
	conf->_blacklist = create_node("n"); //create first node n to split tree into two parts
	FILE* file = fopen(filename, "r");
	if(file == NULL)
	{
		conf->_status = errno;
		return;
	}
	char buff[BUFFSIZE];
	int config_length = 2;
	while(fgets(buff, BUFFSIZE, file) != NULL)
	{
		if(buff[0] != '#' && strlen(buff)-1 > 0)
		{
			buff[strlen(buff)-1] = '\0'; // replace '\n' to '\0'
			switch(config_length)
			{
				case 2: //master dns
					strncpy(conf->_masterdns, buff, MAX_ADDRLEN);
					--config_length;
					break;
				case 1: //if addr blacklisted
					if(strcmp(buff,"noaddr") == 0)
					{
						conf->_blacklistresponse[0] = '\0';
					}
					else
					{
						//load ip's numbers to structure
						sscanf(buff,"%d.%d.%d.%d", &(conf->_blacklistresponse[0]), &(conf->_blacklistresponse[1]), &(conf->_blacklistresponse[2]), &(conf->_blacklistresponse[3]));
					}
					--config_length;
					break;
				case 0:
					tree_add(conf->_blacklist, buff);
			}
		}
	}
	conf->_status = 0;
	fclose(file);
}
