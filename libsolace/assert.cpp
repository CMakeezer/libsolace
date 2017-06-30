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
 *	@file		exception.cpp
 *	@author		$LastChangedBy: $
 *	@date		$LastChangedDate: $
 *	@brief		Implementation of exception types
 *	ID:			$Id: $
 ******************************************************************************/
#include "solace/assert.hpp"
#include "solace/exception.hpp"


using namespace Solace;


uint64 Solace::assertIndexInRange(uint64 index, uint64 from, uint64 to) {
    if (index >= to || index < from) {
        Solace::raise<IndexOutOfRangeException>(index, from, to);
    }

    return index;
}


void Solace::raiseInvalidStateError() {
    Solace::raise<NoSuchElementException>("None");
}

void Solace::raiseInvalidStateError(const char* message) {
    Solace::raise<Exception>(message);
}
