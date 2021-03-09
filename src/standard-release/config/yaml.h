/**
 * @file standard-release/config/yaml.h
 * @brief YAML config files.
 */
#pragma once

#include "standard-release/config/iconfig.h"
#include "standard-release/errors/errors.h"
#include "standard-release/global/global.h"
#include "standard-release/semver/semver.h"
#include <string>

namespace StandardRelease {

class YamlConfigPrivate;

/**
 * @brief Config file parser.
 */
class STANDARDRELEASE_EXPORT YamlConfig : public IConfig
{
public:
    /** Create a new instance. */
    YamlConfig();

    /** Create a new instance from a config file. */
    YamlConfig(const std::string &fileName);

    /**
     * @brief Returns the current config filename.
     * @returns The current path to the YAML config file.
     */
    std::string filename() const;

    /**
     * @brief Sets the new path to a YAML config file.
     * @param fileName Path to a YAML file.
     */
    void setFilename(const std::string &fileName);

    /**
     * @brief Parse the current YAML file and populate configuration values.
     * @returns Result of parsing the YAML file.
     */
    bool parse();

    /**
     * @brief Parse a YAML config file.
     */
    static YamlConfig parse(const std::string &file);

private:
    YamlConfigPrivate *d;
};

}
