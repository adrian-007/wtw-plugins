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

#include "stdinc.hpp"
#include "DNSLookup.hpp"

bool DNSLookup::setDomain(const std::string& domain)
{ 
	this->domain = domain; 
	lookup(); 
	return ips.empty() == false;
}

const std::string& DNSLookup::getDomain() const
{ 
	return this->domain; 
}

std::string DNSLookup::front()
{
	if(ips.empty())
	{
		lookup();
	}

	return ips.empty() ? std::string() : ips.front();
}

void DNSLookup::pop_front()
{
	if(ips.empty() == false)
	{
		ips.pop_front();
	}
}

size_t DNSLookup::size() const
{
	return ips.size();
}

void DNSLookup::clear()
{
	ips.clear();
}

void DNSLookup::lookup()
{
	ips.clear();

	struct addrinfo hints = { 0 };     
	struct addrinfo* target;

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if(getaddrinfo(domain.c_str(), 0, &hints, &target) == 0)
	{
		char printableIP[INET6_ADDRSTRLEN];
		struct addrinfo* grabAllTargetIPs;
		struct sockaddr_in* ipv4;
		void* ipAddress;
		DWORD len;

		for(grabAllTargetIPs = target; grabAllTargetIPs != 0; grabAllTargetIPs = grabAllTargetIPs->ai_next)
		{
			len = sizeof(printableIP);
			memset(printableIP, 0, len);

			ipv4 = reinterpret_cast<struct sockaddr_in*>(grabAllTargetIPs->ai_addr);
			ipAddress = &(ipv4->sin_addr);

			if(::WSAAddressToStringA(reinterpret_cast<struct sockaddr*>(ipv4), sizeof(*ipv4), 0, printableIP, &len) == 0)
			{
				ips.push_back(string(printableIP, len));	
			}
		}

		freeaddrinfo(target);
	}
}
