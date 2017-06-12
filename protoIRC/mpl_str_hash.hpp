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

namespace string_hash
{
typedef unsigned int HashType;

#define MPL_STR_ROTATE_LEFT(val, bits)			( ((val) << ((sizeof(val)*8) - (bits))) | ((val) >> (bits)) )
#define MPL_STR_ROTATE_RIGHT(val, bits)			( ((val) >> ((sizeof(val)*8) - (bits))) | ((val) << (bits)) )
#define MPL_STR_HASH_GET_VALUE(curHash, c)		(HashType)((curHash) ^ (MPL_STR_ROTATE_LEFT(curHash, 9) * c + MPL_STR_ROTATE_RIGHT(curHash, 7)))

#define MPL_STR_HASH_SEED ((HashType)0xAAAAAAAAAAAAAAAAu)

#pragma warning(push)
#pragma warning(disable:4307) // integral constant overflow

	template<const char C1 = 0, const char C2 = 0, const char C3 = 0, const char C4 = 0, const char C5 = 0, const char C6 = 0, const char C7 = 0, const char C8 = 0, const char C9 = 0, const char C10 = 0, const char C11 = 0, const char C12 = 0, const char C13 = 0, const char C14 = 0, const char C15 = 0>
	struct hash
	{
	private:
		template<const char C, HashType hash = MPL_STR_HASH_SEED>
		struct HashCalc
		{
			static const HashType value = (C == 0) ? hash : MPL_STR_HASH_GET_VALUE(hash, C);
		};

	public:
		static const HashType value = HashCalc<C15,HashCalc<C14,HashCalc<C13,HashCalc<C12,HashCalc<C11,HashCalc<C10,HashCalc<C9,HashCalc<C8,HashCalc<C7,HashCalc<C6,HashCalc<C5,HashCalc<C4,HashCalc<C3,HashCalc<C2,HashCalc<C1>::value>::value>::value>::value>::value>::value>::value>::value>::value>::value>::value>::value>::value>::value>::value;
	};

	HashType get_hash(const char* str, size_t len)
	{
		HashType hash = MPL_STR_HASH_SEED;

		for(size_t i = 0; i < len; ++i)
		{
			hash = MPL_STR_HASH_GET_VALUE(hash, str[i]);
		}

		return hash;
	}

	inline HashType get_hash(const char* str)
	{
		return get_hash(str, strlen(str));
	}

#pragma warning(pop)
}
