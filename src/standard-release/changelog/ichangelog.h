/**
 * @file standard-release/changelog/ichangelog.h
 * @brief Interface for changelogs.
 */
#pragma once

#include "standard-release/commits/iconventional.h"
#include "standard-release/errors/errors.h"
#include "standard-release/global/global.h"
#include "standard-release/semver/semver.h"
#include <string>

namespace StandardRelease {

class IChangelogPrivate;

/**
 * @brief Interface for Changelog generators.
 * @details This is the base class for all Changelog formats.
 */
class STANDARDRELEASE_EXPORT IChangelog
{
public:
    /** Initialize a new changelog. */
    IChangelog();
    /** Initialize a new changelog from a file path. */
    IChangelog(const std::string filename);
    ~IChangelog();

    /** Set the current path to the changelog. */
    void setFilename(const std::string filename);

    /**
     * Append a single entry.
     * @param[in] commit Formatted commit to add to the latest changelog.
     */
    void addEntry(const SemVer version, const IConventionalCommit::Commit commit);

    /** Current status. */
    Error error() const;

    /** Path to the file. */
    std::string filename() const;

    /** Does the file currently exist? */
    bool exists() const;

    /** Read an existing changelog. */
    virtual void read() = 0;

    /** Write the changelog to a file. */
    virtual void write() = 0;

    /**
     * @brief Generate the contents of a CHANGELOG file.
     * @details This is the function that renders the Changelog into its
     * final format.
     * @note Must be called AFTER read() but before write()!
     */
    virtual void generate(const SemVer version, const SemVer old,
                          const IConventionalCommit::Commits commits, const std::string origin) = 0;

protected:
    void setContent(const std::string content);
    void setError(const Error error);

    std::string content() const;

    /** List of all newly-added commits. */
    IConventionalCommit::Commits commits() const;

private:
    IChangelogPrivate *d;
};

}
