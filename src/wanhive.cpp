//============================================================================
// Name        : wanhive.cpp
// Author      : Wanhive Systems Private Limited (info@wanhive.com)
// Version     :
// Copyright   : Copyright 2020 Wanhive Systems Private Limited
// License     : Check the COPYING file for the license
// Description : The main function
//============================================================================

#include "app/AppManager.h"

int main(int argc, char *argv[]) {
	wanhive::AppManager::execute(argc, argv);
	return 0;
}
