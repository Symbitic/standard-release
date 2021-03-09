#include "boost/ut.hpp"
#include "semver/semver.h"
#include <vector>

using namespace boost::ut;
using namespace boost::ut::spec;
using namespace StandardRelease;

struct ParseTestData
{
    std::string version;
    int major;
    int minor;
    int patch;
};

struct IncrementTestData
{
    SemVer current;
    SemVer result;
    SemVer::Increment type;
};

const std::vector<ParseTestData> parseTestData = {
    // clang-format off
    { {"0.0.0"}, 0, 0, 0 },
    { {"1.0.0"}, 1, 0, 0 },
    { {"0.1.0"}, 0, 1, 0 },
    { {"0.0.1"}, 0, 0, 1 }
    // clang-format on
};

const std::vector<IncrementTestData> incrementTestData = {
    // clang-format off
    { {0,1,0}, {1,0,0}, SemVer::Major },
    { {2,4,25}, {3,0,0}, SemVer::Major },
    { {1,2,3}, {1,3,0}, SemVer::Minor },
    { {3,0,0}, {3,1,0}, SemVer::Minor },
    { {0,0,99}, {0,0,100}, SemVer::Patch },
    // clang-format on
};

int main()
{
    "SemVer"_test = [] {
        it("should initialize without crashing") = [] {
            const auto fn = [] { SemVer semver; };
            expect(nothrow(fn));
        };

        for (auto testcase : parseTestData) {
            it("should parse " + testcase.version) = [testcase] {
                SemVer v;
                v.parse(testcase.version);
                expect(that % v.major() == testcase.major);
                expect(that % v.minor() == testcase.minor);
                expect(that % v.patch() == testcase.patch);
            };
        };

        for (auto testcase : incrementTestData) {
            const auto from = testcase.current.str();
            const auto to = testcase.result.str();
            it("should increment " + from + " to " + to) = [testcase] {
                SemVer current = testcase.current;
                SemVer result = testcase.result;
                expect(that % result.incrementType(current) == testcase.type)
                        << "guess the correct increment type";
                current.increment(testcase.type);
                expect(that % current == result) << "increment correctly";
            };
        }
    };
}
