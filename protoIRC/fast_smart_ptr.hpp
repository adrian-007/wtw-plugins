/*
 * Copyright (C) 2009-2017 adrian_007, adrian-007 on o2 point pl
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#pragma once

#include <boost/intrusive_ptr.hpp>

template<typename T>
class fast_smart_ptr
{
public:
	fast_smart_ptr(void) : __refCount(0)
	{
	}

	virtual ~fast_smart_ptr(void)
	{
	}

	inline void intrusive_ptr_add_ref(T* p)
	{
		::InterlockedIncrement(&(p->__refCount));
	}

	inline void intrusive_ptr_release(T* p)
	{
		if(::InterlockedDecrement(&(p->__refCount)) <= 0)
		{
			delete static_cast<T*>(this);
		}
	}

	inline void inc()
	{
		intrusive_ptr_add_ref(this);
	}

	inline void dec()
	{
		intrusive_ptr_release(this);
	}

private:
	volatile unsigned long __refCount;
};
