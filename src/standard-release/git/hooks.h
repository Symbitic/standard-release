#pragma once

#include "standard-release/errors/errors.h"
#include "standard-release/global/global.h"
#include <string>

namespace StandardRelease {

/**
 * @brief Install git hooks.
 */
class STANDARDRELEASE_EXPORT GitHooks
{
public:
    GitHooks();

    enum Result
    {
        Success,
        AlreadyInstalled,
        NotAGitRepo,
    };

    Result install(const std::string &dirname);

private:
    Error m_error;
};

};
