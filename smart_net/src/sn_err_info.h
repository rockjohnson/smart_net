/*
 * errors.h
 *
 *  Created on: Feb 12, 2011
 *      Author: rock
 */

#ifndef __ERRORS_H__
#define __ERRORS_H__

#define SMART_NET_ERR_CODE_BEGINNING (-100000) //-100000 ~ -10001

enum
{
	SNERR_DEF = SMART_NET_ERR_CODE_BEGINNING,
	SNERR_CREAT_SOCK_FAILDED,
	SNERR_IN_PROGRESS
};


#endif /* ERRORS_H_ */
