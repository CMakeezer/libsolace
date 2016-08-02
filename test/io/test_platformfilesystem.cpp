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
 * @file: test/io/test_platformFilesystem.cpp
 * @author: soultaker
 *
 * Created on: 20/06/2016
*******************************************************************************/
#include <solace/io/platformFilesystem.hpp>  // Class being tested

#include <solace/memoryManager.hpp>
#include <solace/io/ioexception.hpp>
#include <solace/uuid.hpp>

#include <cppunit/extensions/HelperMacros.h>

#include <unistd.h>
#include <signal.h>
#include <sys/time.h>


using namespace Solace;
using namespace Solace::IO;


class TestPlatformFs : public CppUnit::TestFixture  {

    CPPUNIT_TEST_SUITE(TestPlatformFs);
        CPPUNIT_TEST(testCreation);
        CPPUNIT_TEST(testCreationAndRemoval);
        CPPUNIT_TEST(testGetExecPath);
        CPPUNIT_TEST(testThereIsADirectory);
        CPPUNIT_TEST(testWorkingDirectory);
        CPPUNIT_TEST(testTemp);
    CPPUNIT_TEST_SUITE_END();

protected:
    MemoryManager _memoryManager;

public:

    TestPlatformFs(): _memoryManager(4096)
    {
    }

    void testCreation() {
        const auto fileUID = UUID::random();
        const auto filename = Path::parse(String::join("-", {"test", fileUID.toString()}));
        const auto fileUIDBytes = fileUID.toBytes();


        auto fs = PlatformFilesystem();
        if (fs.exists(filename)) {
            fs.unlink(filename);
        }

        CPPUNIT_ASSERT(!fs.exists(filename));
        {
            auto f = fs.create(filename);
            CPPUNIT_ASSERT(fs.exists(filename));

            MemoryView::size_type bytesWriten = f->write(fileUIDBytes);
            CPPUNIT_ASSERT_EQUAL(fileUIDBytes.size(), bytesWriten);

            f->seek(0, File::Seek::Set);

            auto mem = _memoryManager.create(fileUIDBytes.size());
            ByteBuffer readBuffer(mem);
            MemoryView::size_type bytesRead = f->read(readBuffer);
            CPPUNIT_ASSERT_EQUAL(fileUIDBytes.size(), bytesRead);
            CPPUNIT_ASSERT_EQUAL(false, readBuffer.hasRemaining());
            readBuffer.flip();

            for (ByteBuffer::size_type i = 0; i < readBuffer.limit(); ++i) {
                CPPUNIT_ASSERT_EQUAL(fileUIDBytes[i], readBuffer.get());
            }
        }

        CPPUNIT_ASSERT(fs.unlink(filename));
        CPPUNIT_ASSERT(!fs.exists(filename));
    }

    void testCreationAndRemoval() {
        const auto fileUID = UUID::random();
        const auto filename = Path::parse(String::join("-", {"test", fileUID.toString()}));
        const auto fileUIDBytes = fileUID.toBytes();

        auto fs = PlatformFilesystem();
        if (fs.exists(filename)) {
            fs.unlink(filename);
        }
        CPPUNIT_ASSERT(!fs.exists(filename));

        {
            auto f = fs.create(filename);
            CPPUNIT_ASSERT(fs.exists(filename));
            f->write(fileUIDBytes);

            f->close();

            auto mem = _memoryManager.create(fileUIDBytes.size());
            ByteBuffer readBuffer(mem);
            CPPUNIT_ASSERT_THROW(auto x = f->seek(0, File::Seek::Set), NotOpen);
            CPPUNIT_ASSERT_THROW(auto x = f->read(readBuffer), NotOpen);
        }

        // Attempt to 'create' already existing file
        CPPUNIT_ASSERT_THROW(auto f = fs.create(filename), IOException);
        CPPUNIT_ASSERT_EQUAL(fileUIDBytes.size(), fs.getFileSize(filename));
        CPPUNIT_ASSERT(fs.isFile(filename));
        CPPUNIT_ASSERT(!fs.isDirectory(filename));

        {
            auto f = fs.open(filename);

            auto mem = _memoryManager.create(fileUIDBytes.size());
            ByteBuffer readBuffer(mem);
            MemoryView::size_type bytesRead = f->read(readBuffer);
            CPPUNIT_ASSERT_EQUAL(fileUIDBytes.size(), bytesRead);
            CPPUNIT_ASSERT_EQUAL(false, readBuffer.hasRemaining());
            readBuffer.flip();

            for (ByteBuffer::size_type i = 0; i < readBuffer.limit(); ++i) {
                CPPUNIT_ASSERT_EQUAL(fileUIDBytes[i], readBuffer.get());
            }
        }


        timeval timeOfDay;
        gettimeofday(&timeOfDay, nullptr);
        auto t = fs.getTimestamp(filename);
        CPPUNIT_ASSERT(timeOfDay.tv_sec - t.tv_sec < 2);

        CPPUNIT_ASSERT(fs.unlink(filename));
        CPPUNIT_ASSERT(!fs.exists(filename));
    }

    void testGetExecPath() {
        auto fs = PlatformFilesystem();

        auto pathToThisTest = fs.getExecPath();
        CPPUNIT_ASSERT_EQUAL(String("test_solace"), pathToThisTest.last());
    }

    void testThereIsADirectory() {
        auto fs = PlatformFilesystem();

        auto pathToThisTest = fs.getExecPath();
        auto realPathToThisTest = fs.realPath(pathToThisTest);

        CPPUNIT_ASSERT(fs.isFile(realPathToThisTest));
        CPPUNIT_ASSERT(fs.isDirectory(realPathToThisTest.getParent()));
    }

    void testWorkingDirectory() {
        auto fs = PlatformFilesystem();

        auto cwd = fs.getWorkingDirectory();
        CPPUNIT_ASSERT(fs.isDirectory(cwd));
        CPPUNIT_ASSERT(!fs.isFile(cwd));

        fs.setWorkingDirectory(cwd.getParent());
        CPPUNIT_ASSERT_EQUAL(cwd.getParent(), fs.getWorkingDirectory());

    }

    void testTemp() {
        const auto fileUID = UUID::random();
        const auto fileUIDBytes = fileUID.toBytes();

        auto fs = PlatformFilesystem();
        {
            auto f = fs.createTemp();

            const MemoryView::size_type  bytesWriten = f->write(fileUIDBytes);
            CPPUNIT_ASSERT_EQUAL(fileUIDBytes.size(), bytesWriten);
            CPPUNIT_ASSERT_EQUAL(bytesWriten, static_cast<decltype(bytesWriten)>(f->tell()));

            f->seek(0, File::Seek::Set);

            auto mem = _memoryManager.create(fileUIDBytes.size());
            ByteBuffer readBuffer(mem);
            const MemoryView::size_type bytesRead = f->read(readBuffer);
            CPPUNIT_ASSERT_EQUAL(fileUIDBytes.size(), bytesRead);
            CPPUNIT_ASSERT_EQUAL(false, readBuffer.hasRemaining());
            readBuffer.flip();

            for (ByteBuffer::size_type i = 0; i < readBuffer.limit(); ++i) {
                CPPUNIT_ASSERT_EQUAL(fileUIDBytes[i], readBuffer.get());
            }
        }
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestPlatformFs);
