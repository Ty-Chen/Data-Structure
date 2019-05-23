#pragma once

/*

Copyright (c) 2003, Arvid Norberg
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

	* Redistributions of source code must retain the above copyright
	  notice, this list of conditions and the following disclaimer.
	* Redistributions in binary form must reproduce the above copyright
	  notice, this list of conditions and the following disclaimer in
	  the documentation and/or other materials provided with the distribution.
	* Neither the name of the author nor the names of its
	  contributors may be used to endorse or promote products derived
	  from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

*/

#ifndef TORRENT_HASHER_HPP_INCLUDED
#define TORRENT_HASHER_HPP_INCLUDED

#include "sha1.hpp"
#include "sha1_hash.hpp"
#include <string>
#include <assert.h>

class hasher
{
public:

	hasher()
	{
		SHA1_Init(&m_context);
	}

	hasher(const char* data, int len)
	{
		assert(data != 0);
		assert(len > 0);

		SHA1_Init(&m_context);
		SHA1_Update(&m_context, reinterpret_cast<unsigned char const*>(data), len);
	}

	void update(std::string const& data) { update(data.c_str(), data.size()); }

	void update(const char* data, int len)
	{
		assert(data != 0);
		assert(len > 0);

		SHA1_Update(&m_context, reinterpret_cast<unsigned char const*>(data), len);

	}

	sha1_hash final()
	{
		sha1_hash digest;

		SHA1_Final(digest.begin(), &m_context);

		return digest;
	}

	void reset()
	{
		SHA1_Init(&m_context);
	}

private:

	SHA_CTX m_context;

};


#endif // TORRENT_HASHER_HPP_INCLUDED


