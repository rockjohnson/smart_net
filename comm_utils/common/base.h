/*
 * noncopyable.h
 *
 *  Created on: Apr 5, 2009
 *      Author: rock
 */

#ifndef __NONCOPYABLE_H__
#define __NONCOPYABLE_H__

namespace nm_base
{
	class CNoncopyable
	{
	protected:
		CNoncopyable()
		{
		}
		virtual ~CNoncopyable()
		{
		}
	private:
		// emphasize the following members are private
		CNoncopyable(const CNoncopyable&);
		const CNoncopyable& operator=(const CNoncopyable&);
	};

	class CNonnewobj
	{
	private:
		CNonnewobj()
		{
		}
		CNonnewobj(const CNonnewobj &t)
		{
		}
	public:
		virtual ~CNonnewobj()
		{
		}
	};
}

#endif /* __NONCOPYABLE_H__ */
