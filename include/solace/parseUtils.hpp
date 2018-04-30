/*
*  Copyright 2017 Ivan Ryabov
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
*/
/*******************************************************************************
 * libSolace: Text parsing utils
 *	@file		solace/parseUtils.hpp
 *	@brief		Various text parsing utils that has not found a better home yet.
 ******************************************************************************/
#pragma once
#ifndef SOLACE_PARSEUTILS_HPP
#define SOLACE_PARSEUTILS_HPP

#include "solace/types.hpp"
#include "solace/string.hpp"
#include "solace/result.hpp"
#include "solace/error.hpp"

namespace Solace {

Result<bool, Error> tryParseBoolean(StringView value);

Result<int8, Error> tryParseInt8(StringView value);

Result<int16, Error> tryParseInt16(StringView value);

Result<int32, Error> tryParseInt32(StringView value);

Result<int64, Error> tryParseInt64(StringView value);

Result<uint8, Error> tryParseUInt8(StringView value);

Result<uint16, Error> tryParseUInt16(StringView value);

Result<uint32, Error> tryParseUInt32(StringView value);

Result<uint64, Error> tryParseUInt64(StringView value);


template<typename T>
Result<T, Error> tryParse(StringView value);


template<>
inline
Result<bool, Error> tryParse<bool>(StringView value) { return tryParseBoolean(value); }

template<>
inline
Result<int8, Error> tryParse<int8>(StringView value) { return tryParseInt8(value); }

template<>
inline
Result<int16, Error> tryParse<int16>(StringView value) { return tryParseInt16(value); }

template<>
inline
Result<int32, Error> tryParse<int32>(StringView value) { return tryParseInt32(value); }

template<>
inline
Result<int64, Error> tryParse<int64>(StringView value) { return tryParseInt64(value); }


template<>
inline
Result<uint8, Error> tryParse<uint8>(StringView value) { return tryParseUInt8(value); }

template<>
inline
Result<uint16, Error> tryParse<uint16>(StringView value) { return tryParseUInt16(value); }

template<>
inline
Result<uint32, Error> tryParse<uint32>(StringView value) { return tryParseUInt32(value); }

template<>
inline
Result<uint64, Error> tryParse<uint64>(StringView value) { return tryParseUInt64(value); }


}  // End of namespace Solace
#endif  // SOLACE_PARSEUTILS_HPP
