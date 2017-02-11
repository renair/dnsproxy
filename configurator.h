#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include "tree.h"

#define MAX_ADDRLEN 15

struct config
{
	char _masterdns[MAX_ADDRLEN];
	char _blacklistresponse[MAX_ADDRLEN];
	tree* _blacklist;
	int _status;
};

void load_config(char*, struct config*);

#endif
