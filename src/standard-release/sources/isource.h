/**
 * @file sources/source.h
 * @brief Virtual class for sources.
 *
 * A source is a method for determining the current version, and for saving modified versions.
 */
#pragma once

#include "standard-release/errors/errors.h"
#include "standard-release/global/global.h"
#include "standard-release/semver/semver.h"
#include <string>
#include <vector>

namespace StandardRelease {

class ISourcePrivate;

/**
 * @brief Virtual class for reading/writing a project's version.
 */
class STANDARDRELEASE_EXPORT ISource
{
public:
    /**
     * @brief Create a new instance.
     */
    ISource();

    /** Current filename. */
    std::string filename() const;

    /**
     * @brief Set the current file.
     * @param fileName Path to the new file.
     */
    void setFilename(const std::string &fileName);

    /** The current version. */
    SemVer version() const;

    /** Set a new version. */
    void setVersion(const SemVer &version);

    /** Most recent error. */
    Error error() const;

    virtual bool detect(const std::string &dirname) = 0;
    virtual bool save() = 0;

    virtual std::vector<std::string> filenames() const = 0;

protected:
    virtual bool readFile(const std::string &filename) = 0;

    void setError(const Error error);

private:
    ISourcePrivate *d;
};

};
