#include "semver.h"
#include <regex>
#include <string_view>

using namespace StandardRelease;

SemVer::SemVer()
    : m_major(0)
    , m_minor(0)
    , m_patch(0)
{
}

SemVer::SemVer(int major, int minor, int patch)
    : m_major(major)
    , m_minor(minor)
    , m_patch(patch)
{
}

int SemVer::major() const
{
    return m_major;
}

int SemVer::minor() const
{
    return m_minor;
}

int SemVer::patch() const
{
    return m_patch;
}

void SemVer::increment(Increment inc)
{
    switch (inc) {
        case Increment::Major:
            m_major++;
            m_minor = 0;
            m_patch = 0;
            break;
        case Increment::Minor:
            m_minor++;
            m_patch = 0;
            break;
        case Increment::Patch:
            m_patch++;
            break;
    }
}

SemVer::Increment SemVer::incrementType(const SemVer &v) const
{
    if (major() > v.major()) {
        return Increment::Major;
    } else if (minor() > v.minor()) {
        return Increment::Minor;
    } else if (patch() > v.patch()) {
        return Increment::Patch;
    } else {
        return Increment::None;
    }
}

bool operator>(const SemVer &v1, const SemVer &v2)
{
    if (v1.major() > v2.major()) {
        return true;
    } else if (v1.major() < v2.major()) {
        return false;
    } else if (v1.minor() > v2.minor()) {
        return true;
    } else if (v1.minor() < v2.minor()) {
        return false;
    } else if (v1.patch() > v2.patch()) {
        return true;
    } else if (v1.patch() < v2.patch()) {
        return false;
    } else {
        return false;
    }
}

bool operator>=(const SemVer &v1, const SemVer &v2)
{
    if (v1.major() > v2.major()) {
        return true;
    } else if (v1.major() < v2.major()) {
        return false;
    } else if (v1.minor() > v2.minor()) {
        return true;
    } else if (v1.minor() < v2.minor()) {
        return false;
    } else if (v1.patch() > v2.patch()) {
        return true;
    } else if (v1.patch() < v2.patch()) {
        return false;
    } else {
        return true;
    }
}

bool operator<(const SemVer &v1, const SemVer &v2)
{
    if (v1.major() > v2.major()) {
        return false;
    } else if (v1.major() < v2.major()) {
        return true;
    } else if (v1.minor() > v2.minor()) {
        return false;
    } else if (v1.minor() < v2.minor()) {
        return true;
    } else if (v1.patch() > v2.patch()) {
        return false;
    } else if (v1.patch() < v2.patch()) {
        return true;
    } else {
        return false;
    }
}

bool operator<=(const SemVer &v1, const SemVer &v2)
{
    if (v1.major() > v2.major()) {
        return false;
    } else if (v1.major() < v2.major()) {
        return true;
    } else if (v1.minor() > v2.minor()) {
        return false;
    } else if (v1.minor() < v2.minor()) {
        return true;
    } else if (v1.patch() > v2.patch()) {
        return false;
    } else if (v1.patch() < v2.patch()) {
        return true;
    } else {
        return true;
    }
}

bool SemVer::operator==(const SemVer &v)
{
    return major() == v.major() && minor() == v.minor() && patch() == v.patch();
}

bool SemVer::operator!=(const SemVer &v)
{
    return !(major() == v.major() && minor() == v.minor() && patch() == v.patch());
}
/*
void SemVer::operator=(const SemVer &v)
{
    m_major = v.m_major;
    m_minor = v.m_minor;
    m_patch = v.m_patch;
}
*/

bool SemVer::parse(const std::string &version)
{
    std::smatch match;
    std::regex_search(version, match, std::regex(FULL_VERSION));
    if (match.empty()) {
        return false;
    }

    try {
        m_major = +(std::stoi(match[1]));
        m_minor = +(std::stoi(match[2]));
        m_patch = +(std::stoi(match[3]));
    } catch (const std::exception &e) {
        return false;
    }

    return true;
}

std::string SemVer::str() const
{
    std::stringstream ss;
    ss << m_major << '.' << m_minor << '.' << m_patch;
    return ss.str();
}

SemVer::operator const std::string() const
{
    return str();
}

// TODO: How do I get rid of namespace?
namespace StandardRelease {
std::ostream &operator<<(std::ostream &out, const SemVer &v)
{
    return out << v.str();
}
}
