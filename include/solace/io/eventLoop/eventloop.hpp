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
 * libSolace: Unix File handle wrapper
 *	@file		solace/io/file.hpp
 *	@author		$LastChangedBy$
 *	@date		$LastChangedDate$
 *	ID:			$Id$
 ******************************************************************************/
#pragma once
#ifndef SOLACE_IO_EVENTLOOP_EVENTLOOP_HPP
#define SOLACE_IO_EVENTLOOP_EVENTLOOP_HPP

#include "solace/types.hpp"
#include "solace/io/selector.hpp"
#include "solace/io/eventLoop/channel.hpp"

#include <memory>

namespace Solace { namespace IO { namespace EventLoop {

/**
 * Event Loop build on top of selectors.
 *
 * This package allows developers to use heigher level concept of the event loop to write reactive applicatoins.
 * Event loop abstacts data sources, polling all inputs and outputs and
 * triggers all event handlers when a channel is ready.
 *
 */
class EventLoop {
public:
//    typedef
public:

    explicit EventLoop(uint32 size);
    EventLoop(EventLoop&& rhs);
    ~EventLoop();

    EventLoop& operator= (EventLoop&& rhs);
    EventLoop& swap(EventLoop& rhs);

    void add(const std::shared_ptr<Channel>& channel);
    void remove(const std::shared_ptr<Channel>& channel);

    /**
     * Run a single iteration of the loop
     * @return True, if there are still more iteration to run
     */
    bool iterate();

    void run();

private:

    Selector _selector;

};

}  // End of namespace EventLoop
}  // End of namespace IO
}  // End of namespace Solace
#endif  // SOLACE_IO_EVENTLOOP_EVENTLOOP_HPP

