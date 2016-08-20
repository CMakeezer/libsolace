/*
*  Copyright 2016 Ivan Ryabov
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
 * libSolace: Assertion helpers
 *	@file		solace/assert.hpp
 *	@author		$LastChangedBy$
 *	@date		$LastChangedDate$
 *	ID:			$Id$
 ******************************************************************************/
#pragma once
#ifndef SOLACE_ASSERT_HPP
#define SOLACE_ASSERT_HPP

#include "solace/types.hpp"


namespace Solace {

/**
 * Throw an error of invalidState
 */
void raiseInvalidStateError();

/**
 * Assert that the give index is within the give range. Throw if it is not.
 * @param index Index value to be asserted.
 * @param from Lower bound (inclusive)
 * @param to Upper value bound (exclusive)
 * @return Index value if the index is in range. Throws otherwise.
 */
uint32 assertIndexInRange(uint32 index, uint32 from, uint32 to);


/**
 * Assert that the give index is within the give range. Throw if it is not.
 * @param index Index value to be asserted.
 * @param from Lower bound (inclusive)
 * @param to Upper value bound (exclusive)
 * @return Index value if the index is in range. Throws otherwise.
 */
uint64 assertIndexInRange(uint64 index, uint64 from, uint64 to);


}  // End of namespace Solace
#endif  // SOLACE_ASSERT_HPP

