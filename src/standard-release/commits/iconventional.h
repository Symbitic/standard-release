/**
 * @file "standard-release/commits/iconventional.h"
 * @brief Interface for Conventional Commits.
 */
#pragma once

#include "standard-release/errors/errors.h"
#include "standard-release/git/repository.h"
#include "standard-release/global/global.h"
#include "standard-release/semver/semver.h"
#include <list>
#include <string>
#include <vector>

namespace StandardRelease {

/**
 * @brief Interface for Conventional Commits.
 * @details This is the base class for all different forms and versions of
 * the [Conventional Commit](https://conventionalcommits.org/) standard.
 */
class STANDARDRELEASE_EXPORT IConventionalCommit
{
public:
    IConventionalCommit();
    ~IConventionalCommit();

    struct Commit
    {
        std::string type;
        std::string scope;
        std::string subject;
        std::string hash;
        bool breaking;
        bool feature;
        bool bugfix;

        Commit(const std::string type, const std::string scope, const std::string subject,
               const std::string hash = "", bool breaking = false, bool feature = false,
               bool bugfix = false)
        {
            this->type = type;
            this->scope = scope;
            this->subject = subject;
            this->hash = hash;
            this->breaking = breaking;
            this->feature = feature;
            this->bugfix = bugfix;
        }
    };

    using Commits = std::vector<IConventionalCommit::Commit>;

    /** Set the current version. */
    void setVersion(const SemVer &semver);
    /** Get the current version. */
    SemVer version() const;
    /** Returns `true` if commits meet the conventionalcommit.org standard; `false` otherwise. */
    bool isValid() const;
    /** Current status. */
    Error error() const;

    /** Parse commit messages for standard compliance. */
    virtual bool parseCommits(const GitRepository::Commits commits) = 0;

    /** Bump the current version based on commits. */
    virtual void bump() = 0;

    Commits commits() const;

protected:
    void setValid(bool valid);
    void setCommits(const Commits commits);
    void setError(const Error error);

private:
    bool m_valid;
    Error m_error;
    Commits m_commits;
    SemVer m_semver;
};

}
