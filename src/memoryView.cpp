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
 * libSolace
 *	@file		memoryView.cpp
 *	@author		$LastChangedBy: soultaker $
 *	@date		$LastChangedDate$
 *	@brief		Implementation of MemoryView
 *	ID:			$Id$
 ******************************************************************************/
#include "solace/memoryView.hpp"
#include "solace/exception.hpp"


#include <utility>
#include <cstring>  // memcpy


using Solace::byte;
using Solace::MemoryView;


// FIXME: To be removed
void delFree(MemoryView* view) {
    delete [] view->dataAddress();
}



MemoryView MemoryView::wrap(byte* data, size_type size, const std::function<void(MemoryView*)>& freeFunction) {
    return MemoryView{size, data, freeFunction};
}


//MemoryView MemoryView::copy(const byte* data, size_type size) {
//    auto dataAddress = new value_type[size];

//    memcpy(dataAddress, data, size * sizeof(value_type));

//    return MemoryView{size, dataAddress, delFree};
//}



//MemoryView::MemoryView(size_type newSize):
////    _size(newSize), _ownsData(true), _dataAddress(new value_type[newSize])
//  _size(newSize), _dataAddress(new value_type[newSize]), _free(delFree)
//{
//}


MemoryView::MemoryView(const MemoryView& rhs):
            _size(rhs._size),
//            _ownsData(rhs._ownsData),
            _dataAddress(rhs._dataAddress),
            _free(0)
{
//    if (rhs._dataAddress && rhs._ownsData) {
//        _dataAddress = new value_type[_size];

//        memcpy(_dataAddress, rhs._dataAddress, _size * sizeof(value_type));
//    }
}


MemoryView::MemoryView(MemoryView&& rhs) noexcept :
            _size(rhs._size),
//            _ownsData(rhs._ownsData),
            _dataAddress(rhs._dataAddress),
            _free(std::move(rhs._free))
{
    // Stuff up rhs so it won't be destructed
    rhs._size = 0;
//    rhs._ownsData = false;
    rhs._dataAddress = nullptr;
    rhs._free = 0;
}


//MemoryView::MemoryView(size_type newSize, void* data, bool takeOwnership, bool copyData) :
//    _size(newSize),
//    _ownsData(takeOwnership),
//    _dataAddress(reinterpret_cast<value_type*>(data))
//{
//    if (!bytes) {
////         FIXME(abbyssoul): Review nullptr handling policy
//        if (_size)
//            raise<IllegalArgumentException>("data");
////        _size = 0;
//    }

////    if (bytes && copyData) {
////        _dataAddress = new value_type[_size];

////        memcpy(_dataAddress, bytes, _size * sizeof(value_type));
////    }
//}


MemoryView::MemoryView(size_type newSize, void* data, const std::function<void(MemoryView*)>& freeFunc) :
    _size(newSize),
//    _ownsData(takeOwnership),
    _dataAddress(reinterpret_cast<value_type*>(data)),
    _free(freeFunc)
{
    if (!_dataAddress && _size) {
        raise<IllegalArgumentException>("data");
    }
}



MemoryView::~MemoryView() {
    if (_free) {
        _free(this);

        _dataAddress = nullptr;
        _size = 0;
    }

//    if (_ownsData && _dataAddress) {
//        _ownsData = false;

//        delete [] _dataAddress;

//        _dataAddress = 0;
//        _size = 0;
//    }
}

MemoryView::reference  MemoryView::operator[] (size_type index) {
    if (index >= size()) {
        raise<IndexOutOfRangeException>("index", index, 0, size());
    }

    return _dataAddress[index];
}

MemoryView::value_type MemoryView::operator[] (size_type index) const {
    if (index >= size()) {
        raise<IndexOutOfRangeException>("index", index, 0, size());
    }

    return _dataAddress[index];
}


byte* MemoryView::dataAddress(size_type offset) const {
    if (offset >= size()) {
        raise<IndexOutOfRangeException>("offset", offset, 0, size());
    }

    return _dataAddress + offset;
}


MemoryView& MemoryView::swap(MemoryView& rhs) noexcept {
    std::swap(_size, rhs._size);
//    std::swap(_ownsData, rhs._ownsData);
    std::swap(_dataAddress, rhs._dataAddress);
    std::swap(_free, rhs._free);

    return (*this);
}


MemoryView MemoryView::slice(size_type from, size_type to) const {
    if (from >= size()) {
        raise<IndexOutOfRangeException>("from", from, 0, size());
    }

    if (to < from) {
        raise<IndexOutOfRangeException>("to", to, from, size());
    }

    if (to > size()) {
        raise<IndexOutOfRangeException>("to", to, from, size());
    }

//    return (copyData)
//            ? copy(_dataAddress + from, to - from)
//            : wrap(_dataAddress + from, to - from, false);

    return wrap(dataAddress(from), to - from, false);
}


MemoryView& MemoryView::fill(byte value, size_type from, size_type to) {
    if (from >= size()) {
        raise<IndexOutOfRangeException>("from", from, 0, size());
    }

    if (to < from) {
        raise<IndexOutOfRangeException>("to", to, from, size());
    }

    if (to > size()) {
        raise<IndexOutOfRangeException>("to", to, from, size());
    }

    memset(dataAddress(from), value, to - from);

    return (*this);
}

MemoryView& MemoryView::fill(byte value) {
    memset(_dataAddress, value, _size);

    return (*this);
}
