/**
 * @file standard-release/config/iconfig.h
 * @brief Configuration interface.
 */
#pragma once

#include "standard-release/commits/iconventional.h"
#include "standard-release/errors/errors.h"
#include "standard-release/global/global.h"
#include "standard-release/semver/semver.h"
#include <map>
#include <string>
#include <vector>

namespace StandardRelease {

class IConfigPrivate;

/**
 * @brief Interface for configuration sources.
 */
class STANDARDRELEASE_EXPORT IConfig
{
public:
    /** Subscript operator. */
    std::string operator[](const std::string key);

    /** List of named configuration options. */
    std::vector<std::string> keys() const;

    std::string value(const std::string &key) const;

    /** Current status. */
    Error error() const;

    /**
     * @brief Parse the configuration source.
     * @returns Parsing result.
     */
    virtual bool parse() = 0;

protected:
    /** Create a new instance. */
    IConfig();

    void setError(const Error error);

    void setData(const std::map<std::string, std::string> data);

private:
    IConfigPrivate *d;
};

}
