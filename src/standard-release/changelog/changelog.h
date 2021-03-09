/**
 * @file "standard-release/changelog/changelog.h"
 * @brief Conventional changelog generator.
 */
#pragma once

#include "standard-release/changelog/ichangelog.h"
#include "standard-release/errors/errors.h"
#include <string>

namespace StandardRelease {

class ChangelogPrivate;

/**
 * @brief Conventional Changelog generator.
 * @details This generator follows the format recommended by
 * [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).
 */
class STANDARDRELEASE_EXPORT Changelog : public IChangelog
{
public:
    Changelog();
    Changelog(const std::string filename);
    ~Changelog();

    void read();
    void write();
    void generate(const SemVer version, const SemVer old,
                  const IConventionalCommit::Commits commits, const std::string url);

private:
    void readFile();
    void createFile();

    ChangelogPrivate *d;
};

}
