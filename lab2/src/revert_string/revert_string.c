#include <string.h>

#include "revert_string.h"

void RevertString(char *str)
{
	int l = strlen(str);
	for (int i = 0; i < (int)(l / 2); i++){
		char temp = str[i];
		str[i] = str[l - i - 1];
		str[l - i - 1] = temp;
	}
}

