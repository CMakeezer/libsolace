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
 * libSolace Unit Test Suit
 * @file: test/test_writeBuffer.cpp
 * @author: soultaker
 *
 * Created on: 28 Apr 2016
*******************************************************************************/
#include <solace/io/selector.hpp>  // Class being tested

#include <solace/io/file.hpp>
#include <cppunit/extensions/HelperMacros.h>

#include <unistd.h>
#include <fcntl.h>


using namespace Solace;
using namespace Solace::IO;


class TestIOSelector : public CppUnit::TestFixture  {

    CPPUNIT_TEST_SUITE(TestIOSelector);
        CPPUNIT_TEST(testSubscription);
        CPPUNIT_TEST(testEmptyPolling);
        CPPUNIT_TEST(testRemoval);
    CPPUNIT_TEST_SUITE_END();

public:

    void testSubscription() {
        int pipe_fds[2];
        const int pipeResult = pipe(pipe_fds);
        CPPUNIT_ASSERT_EQUAL(0, pipeResult);

        File read = File::fromFd(pipe_fds[0]);
        File write = File::fromFd(pipe_fds[1]);

        Selector s = Selector::epoll(5);
        s.add(&read, Selector::Read);
        s.add(&write, Selector::Write);

        auto i = s.poll(1);
        CPPUNIT_ASSERT(i != i.end());
        CPPUNIT_ASSERT_EQUAL(static_cast<uint>(1), i.getSize());

        auto ev = *i;
        CPPUNIT_ASSERT_EQUAL(static_cast<ISelectable*>(&write), ev.pollable);
    }

    void testEmptyPolling() {
        Selector s = Selector::epoll(3);

        auto i = s.poll(1);
        CPPUNIT_ASSERT(!(i != i.end()));
        CPPUNIT_ASSERT_EQUAL(static_cast<uint>(0), i.getSize());
        CPPUNIT_ASSERT_THROW(++i, IndexOutOfRangeException);
    }

    void testRemoval() {
        int pipe_fds[2];
        const int pipeResult = pipe(pipe_fds);
        CPPUNIT_ASSERT_EQUAL(0, pipeResult);

        File read = File::fromFd(pipe_fds[0]);
        File write = File::fromFd(pipe_fds[1]);

        Selector s = Selector::epoll(5);
        s.add(&read, Selector::Read);
        s.add(&write, Selector::Write);

        {
            auto i = s.poll(1);
            CPPUNIT_ASSERT(i != i.end());
            CPPUNIT_ASSERT_EQUAL(static_cast<uint>(1), i.getSize());

            auto ev = *i;
            CPPUNIT_ASSERT_EQUAL(static_cast<ISelectable*>(&write), ev.pollable);
        }

        {
            s.remove(&write);
            auto i = s.poll(1);
            CPPUNIT_ASSERT(i == i.end());
            CPPUNIT_ASSERT_EQUAL(static_cast<uint>(0), i.getSize());
        }
    }


};

CPPUNIT_TEST_SUITE_REGISTRATION(TestIOSelector);
