#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include "tree.h"

struct config
{
	char* _masterdns;
	char* _blacklistresponse;
	tree* _blacklist;
	int _status;
};

void load_config(char*, struct config*);

#endif
