/*
 * containers.h
 *
 *  Created on: Jul 2, 2010
 *      Author: rock
 */

#ifndef CONTAINERS_H_
#define CONTAINERS_H_

#ifdef __GXX_EXPERIMENTAL_CXX0X__
#include <unordered_map>
#include <unordered_set>

#define STD_MAP std::unordered_map
#define STD_SET std::unordered_set
#else
#include <map>
#include <set>

#define STD_MAP std::map
#define STD_SET std::set
#endif


#endif /* CONTAINERS_H_ */
