#pragma once

#include "stdafx.h"

typedef struct stdReader_s {

	pipe* output;
	HANDLE thread;

} stdReader;