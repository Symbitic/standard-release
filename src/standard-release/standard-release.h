/**
 * @file standard-release/standard-release.h
 * @brief Entry-point for standard-release.
 */

#pragma once

#include "standard-release/errors/errors.h"
#include "standard-release/global/global.h"
#include <string>

namespace StandardRelease {

class MainPrivate;

/**
 * @brief Main driver for standard release.
 */
class STANDARDRELEASE_EXPORT Main
{
public:
    /**
     * @brief Create a new driver.
     */
    Main();

    /**
     * @brief Create a new driver for a given repository.
     */
    Main(const std::string &dirName);

    /**
     * Supported release types.
     */
    enum ReleaseType
    {
        /** No release type selected yet. */
        None,
        /** CMakeLists.txt */
        CMake,
        /** VERSION.txt */
        TextFile,
        /** package.json */
        Node,
    };

    /**
     * @brief Most recent error.
     */
    Error error() const;

    /**
     * @brief Repository.
     */
    std::string directory() const;

    /**
     * @brief Set the repository location.
     */
    void setDirname(const std::string &dirName);

    /**
     * @brief Open and read a config file.
     * @param fileName Configuration file. If empty, the repository directory is searched.
     */
    void readConfigFile(const std::string &fileName = "");

    /**
     * @brief Initialize a new repository.
     * @details Installs the githooks and creates `.release.yml` if not found.
     * @returns true if successful, false otherwise.
     */
    bool init();

    /**
     * @brief Called by hooks to check if commit is conventional commit format.
     * @returns true if successful, false otherwise.
     */
    bool lint();

    /**
     * @brief Create a new release.
     * @returns true if successful, false otherwise.
     */
    bool release();

private:
    std::string scanForConfigFile(const std::string &dirName);

    std::string readFile(const std::string &fileName);

    MainPrivate *d;
};

};
