/**********************************************************
 * @file   main.h
 * @author jan
 * @date   16.06.17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_MAIN_H
#define NOVELIST_MAIN_H

#include <QtTest>
#include <QObject>
#include <memory>
#include <map>
#include <string>

namespace TestCollector {
    /**
     * List of test suites
     */
    using TestList = std::set<std::unique_ptr<QObject>>;

    /**
     * @return List of all test suites
     */
    inline TestList& getTestList()
    {
        static TestList list;
        return list;
    }

    /**
     * Runs all test suites
     * @param argc Argument count
     * @param argv Arguments
     * @return Number of failed test suites
     */
    inline size_t runAll(int argc, char** argv)
    {
        size_t total = getTestList().size();
        size_t failed = 0;
        for (auto const& i : getTestList())
        {
            auto ret = QTest::qExec(i.get(), argc, argv);
            if(ret != 0)
                failed++;
        }
        std::cout << "Test Suites: " << total - failed << " passed, " << failed << " failed." << std::endl;
        return failed;
    }

    /**
     * Registers a test suite
     * @tparam T Test class
     */
    template<class T>
    class TestRegistrator {
    public:
        TestRegistrator()
        {
            auto& testList = TestCollector::getTestList();
            testList.insert(std::make_unique<T>());
        }
    };
}

/**
 * Register a test suite
 */
#define TEST_SUITE(className) static TestCollector::TestRegistrator<className> s_testRegistrator;

#endif //NOVELIST_MAIN_H
