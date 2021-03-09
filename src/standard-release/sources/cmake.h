#ifndef STANDARDRELEASE_CMAKE_H
#define STANDARDRELEASE_CMAKE_H

#include "standard-release/semver/semver.h"
#include <string>

/**
 * @brief CMakeLists.txt PROJECT() version.
 */
class CMakeFile
{
public:
    CMakeFile();
    CMakeFile(const std::string &filename);

    /** Read VERSION.txt from the filesystem. */
    bool read();
    /** Write the modified file back to disk. */
    bool write();

    void setVersion(SemVer &version);

    std::string version() const;
};

#endif
