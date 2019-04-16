/*
*	High-level wrapper to include correct environment config
*
*	Author: Josh Perry <josh.perry245@gmail.com>
*	Created: 04/13/2019
*   Copyright 2019
*/

#ifndef Constants
#define Constants

#define PROD 0
#define DEV 1
#define NODE_DEV 2

// Set ENV here
#define ENV NODE_DEV

#if ENV == PROD
    #include "./ProdConfig.h"
#elif ENV == DEV
    #include "./DevConfig.h"
#elif ENV == NODE_DEV
    #include "./DevNodeMCUConfig.h"
#else
    #error No ENV defined
#endif

#endif