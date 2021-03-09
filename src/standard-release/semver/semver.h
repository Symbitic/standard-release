/**
 * @file standard-release/semver/semver.h
 * @brief Semantic versioning.
 */
#pragma once

#include <string>

#include "standard-release/global/global.h"

namespace StandardRelease {

// Kept here for now because sources use VERSION.
// TODO: Find a better way to extract/set version in sources.
static const std::string NUMERIC = "0|[1-9]\\d*";
static const std::string NONNUMERIC = "\\d*[a-zA-Z-][a-zA-Z0-9-]*";
static const std::string PRERELEASE_STR = "(?:" + NUMERIC + "|" + NONNUMERIC + ")";
static const std::string PRERELEASE = "(?:-(" + PRERELEASE_STR + "(?:\\." + PRERELEASE_STR + ")*))";
static const std::string BUILD_STR = "[0-9A-Za-z-]+";
static const std::string BUILD = "(?:\\+(" + BUILD_STR + "(?:\\." + BUILD_STR + ")*))";
static const std::string VERSION = "(" + NUMERIC + ")\\.(" + NUMERIC + ")\\.(" + NUMERIC + ")";
static const std::string FULL_VERSION = "^v?" + VERSION + PRERELEASE + "?" + BUILD + "?$";

/**
 * @class SemVer
 * @brief Semantic version.
 *
 * This class encapsulates the logic of a semantic version number, including parsing, incrementing,
 * and formatting.
 */
class STANDARDRELEASE_EXPORT SemVer
{
public:
    /**
     * @brief Construct an empty version object.
     */
    SemVer();

    /**
     * @brief Create a version from version numbers.
     * @param major Version major.
     * @param minor Version minor.
     * @param patch Version patch number.
     */
    SemVer(int major, int minor, int patch);

    /**
     * @brief Returns the version major.
     */
    int major() const;

    /**
     * @brief Returns the version minor.
     */
    int minor() const;

    /**
     * @brief Returns the version patch number.
     */
    int patch() const;

    /**
     * @brief Different ways to increment the version number.
     */
    enum Increment
    {
        /** Increment the major (`1.2.3` -> `2.0.0` ). */
        Major,
        /** Increment the minor (`1.2.1` -> `1.3.0` ). */
        Minor,
        /** Increment the patch (`1.2.0` -> `1.2.1` ). */
        Patch,
        /** No increment was applied. */
        None,
    };

    /**
     * @brief Increment to the next specified version.
     * @param[in] inc Which increment to apply.
     */
    void increment(Increment inc);

    /**
     * @brief Compares two versions to determine which kind of increment was
     * applied.
     * @param[in] v Previous version.
     * @returns Which increment was performed.
     */
    Increment incrementType(const SemVer &v) const;

    /**
     * @brief Parse a version string.
     * @param[in] version SemVer-compatible string (e.g. "1.2.3").
     * @returns `true` if parsed correctly, `false` otherwise.
     */
    bool parse(const std::string &version);

    /**
     * @brief Convert to a string.
     */
    std::string str() const;

    /** @brief String conversion operator. */
    operator const std::string() const;

    /** @brief Input operator. */
    friend std::ostream &operator<<(std::ostream &out, const SemVer &v);

    /** @brief Move operator. */
    // void operator=(const SemVer &v);
    /** eq operator. */
    bool operator==(const SemVer &v);
    /** ne operator. */
    bool operator!=(const SemVer &v);

    /** gt operator. */
    friend bool operator>(const SemVer &v1, const SemVer &v2);
    /** gte operator. */
    friend bool operator>=(const SemVer &v1, const SemVer &v2);
    /** lt operator. */
    friend bool operator<(const SemVer &v1, const SemVer &v2);
    /** lte operator. */
    friend bool operator<=(const SemVer &v1, const SemVer &v2);

private:
    int m_major;
    int m_minor;
    int m_patch;
};

}
