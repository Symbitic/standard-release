#include "conventional.h"
#include <iostream>
#include <regex>
#include <vector>

using namespace StandardRelease;

static const std::string COMMIT = "^([a-zA-Z]+)(\\([a-zA-Z]+\\))?(!?): ([^\\n]+)$";
static const std::string TYPES = "^(build|ci|chore|docs|feat|fix|perf|refactor|revert|style|test)$";

inline std::vector<std::string> split(const std::string &s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream stream(s);
    while (std::getline(stream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

ConventionalCommits::ConventionalCommits()
    : IConventionalCommit()
{
}

bool ConventionalCommits::parseCommits(const GitRepository::Commits commits)
{
    GitRepository::Commits gitcommits = commits;
    Commits conventionalcommits;

    for (const auto gitcommit : gitcommits) {
        const auto gitsummary = gitcommit.summary;
        const auto gitbody = gitcommit.body;
        const auto githash = gitcommit.hash;

        std::smatch match;
        std::regex_search(gitsummary, match, std::regex(COMMIT));
        if (match.empty()) {
            // Not conventional commit format. Skipping.
            continue;
        }

        const std::string typestr = match[1];
        std::string scope = match[2];
        bool breaking = (match[3] == "!");
        std::string subject = match[4];

        // Remove "()" from scope.
        if (!scope.empty()) {
            scope = scope.substr(1, scope.length() - 2);
        }

        // Only store commits AFTER `chore(release): x.y.z`
        if (typestr == "chore" && scope == "release") {
            // TODO: Maybe use version stored in subject somehow?
            break;
        }

        // Check if type is valid.
        std::regex_search(typestr, match, std::regex(TYPES));
        if (match.empty()) {
            setError(Error(Error::ConventionalUnrecognizedType, typestr));
            return false;
        }

        // Parse body.
        std::string body = "";
        bool parsebody = false;
        if (!gitbody.empty()) {
            std::vector lines = split(gitbody, '\n');
            std::reverse(lines.begin(), lines.end());
            for (auto line : lines) {
                if (!parsebody) {
                    if (line.find("BREAKING CHANGE: ") != std::string::npos) {
                        breaking = true;
                    } else if (line.find("BREAKING-CHANGE: ") != std::string::npos) {
                        breaking = true;
                    } else if (line == "") {
                        parsebody = true;
                        continue;
                    }
                } else {
                    // TODO: Maybe handle if line.length() > 100 (?)
                }
            }
        }

#if 0
        std::cout << "+  " << typestr << std::endl;
        if (!scope.empty())
            std::cout << "   " << scope << std::endl;
        std::cout << "   " << subject << std::endl;
        if (breaking)
            std::cout << "   BREAKING CHANGE" << std::endl;
#endif

        Commit commit(typestr, scope, subject);
        commit.breaking = breaking;
        commit.feature = commit.type == "feat" ? true : false;
        commit.bugfix = commit.type == "fix" ? true : false;
        commit.hash = githash;
        conventionalcommits.push_back(commit);
    }

    std::reverse(conventionalcommits.begin(), conventionalcommits.end());

    setCommits(conventionalcommits);

    setError(Error::Success);
    return false;
}

void ConventionalCommits::bump()
{
    bool major = false;
    bool minor = false;
    bool patch = false;
    SemVer v = version();

    for (auto commit : commits()) {
        if (commit.breaking) {
            major = true;
        } else if (commit.feature) {
            minor = true;
        } else if (commit.bugfix) {
            patch = true;
        }
    }

    if (major) {
        v.increment(SemVer::Major);
    } else if (minor) {
        v.increment(SemVer::Minor);
    } else if (patch) {
        v.increment(SemVer::Patch);
    }

    if (v != version()) {
        setVersion(v);
    }
};
