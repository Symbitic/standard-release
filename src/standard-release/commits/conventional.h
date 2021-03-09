#pragma once

#include "standard-release/errors/errors.h"
#include "standard-release/global/global.h"
#include "standard-release/git/repository.h"
#include "standard-release/semver/semver.h"
#include "standard-release/commits/iconventional.h"
#include <list>
#include <string>
#include <vector>

namespace StandardRelease {

/**
 * @brief Conventional Commits.
 */
class STANDARDRELEASE_EXPORT ConventionalCommits : public IConventionalCommit
{
public:
    ConventionalCommits();

    bool parseCommits(const GitRepository::Commits commits);

    /** Bump the current version based on commits. */
    void bump();
};

};
