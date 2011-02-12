/*
 * err_info.h
 *
 *  Created on: Feb 12, 2011
 *      Author: rock
 */

#ifndef __ERR_INFO_H__
#define __ERR_INFO_H__

enum
{
	CMNER_SUC = 0, ///no error.
	CMNER_INVALID_PARAMS = CMNER_SUC - 1, ///invalid parameters.
	CMNER_NO_MATCH_ITEM = CMNER_SUC -2 ///no matched item.
};


#endif /* ERR_INFO_H_ */
