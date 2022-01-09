#include <Windows.h>
#include "publicUtils.h"

int PublicUtils::checkAccountName(string acc) {
	for (int i = 0; i < acc.length(); i++)
	{
		if (isdigit(acc.at(i)) || isalpha(acc.at(i)) || acc.at(i) == '_')
		{
			continue;
		}
		else {
			return FALSE;
		}
	}

	return TRUE;
}