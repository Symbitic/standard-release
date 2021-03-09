#include "boost/ut.hpp"
#include "standard-release/changelog/changelog.h"
#include "standard-release/commits/iconventional.h"
#include "standard-release/errors/errors.h"
#include "standard-release/semver/semver.h"
#include <iostream>

#include <regex>

#define CHANGELOG_DIR FIXTURES_DIR "changelogs/"

using namespace boost::ut;
using namespace boost::ut::spec;
using namespace StandardRelease;

class TestChangelog : public Changelog
{
public:
    TestChangelog()
        : Changelog()
    {
    }

    TestChangelog(const std::string filename)
        : Changelog(filename) {};

    std::string data() const
    {
        return content();
    }
};

struct TestData
{
    std::string filename;
};

struct GenerateTestData
{
    std::string desc;
    std::string filename;
    SemVer current;
    SemVer previous;
    std::string url;
    IConventionalCommit::Commits commits;
};

const std::vector<TestData> testdata = {
    // clang-format off
    { {FIXTURES_DIR "CHANGELOG-1.md"} }
    // clang-format on
};

const std::vector<GenerateTestData> testdata2 = {
    // clang-format off
    {
        {"create a new file with a feature"},
        { CHANGELOG_DIR "create-new.md" },
        {1,0,0},
        {0,2,2},
        { "https://github.com/Symbitic/QSqlTest" },
        {
            { "feat", "lang", "add polish language", "", false, true, false },
            { "fix", "", "correct minor typos in code", "", false, false, true },
            { "feat", "", "major refactor of JSON schema", "7b9af4b", false, true, false },
            { "fix", "", "handle trailing commas in csv header", "cc2ce84", false, false, true },
        }
    }
    // clang-format on
};

int main()
{
    // Remove dates.
    std::regex re(" \\([0-9]{4}-[0-9]{2}-[0-9]{2}\\)\n");

    "Changelog"_test = [] {
        it("should initialize without crashing") = [] {
            const auto fn = [] { Changelog changelog; };
            expect(nothrow(fn));
        };

        for (auto testcase : testdata) {
            it("should read " + testcase.filename) = [testcase] {
                TestChangelog log(testcase.filename);
                log.read();
                expect(neq(log.error(), false)) << "no errors while reading";
            };
        };

        // TESTS:
        //   + Should error if read() with no file?

        for (auto testcase : testdata2) {
            it("should " + testcase.desc) = [testcase] {
                TestChangelog log;
                log.read();
                log.generate(testcase.current, testcase.previous, testcase.commits, testcase.url);
                expect(that % log.error() == Error::Success)
                        << "there were no errors while generating";
                expect(that % log.data().length() > static_cast<unsigned long>(0))
                        << "the data has been generated";

                //
                // TODO: Read and compare to the file.
            };
        }
    };
}
