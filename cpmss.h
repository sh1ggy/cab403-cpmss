/*****************************************************************//**
 * \file   cpmss.h
 * \brief  API definition for the whole system.
 * 
 * \author Anna Nguyenm, Peter Nguyen, Tyrone Nolasco
 * \date   October 2021
 *********************************************************************/
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>    

void *managerMain ( );

void *simulatorMain ( );
