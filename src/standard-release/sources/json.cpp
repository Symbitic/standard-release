#include "json.h"
#include "standard-release/semver/semver.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <system_error>

using namespace StandardRelease;

static const std::string JSON_FIELD = "\"version\": \"([^\"]+)\"";

struct StandardRelease::JsonFilePrivate
{
    std::string contents;
    size_t len;
    size_t pos;
};

JsonFile::JsonFile()
    : ISource()
    , d(new JsonFilePrivate) {};

bool JsonFile::readFile(const std::string &fileName)
{
    std::ifstream in(fileName, std::ios::in | std::ios::binary);
    std::ostringstream contents;
    std::smatch match;
    SemVer version;

    if (!in) {
        setError(Error(Error::PathNotFound, fileName));
        return false;
    }
    contents << in.rdbuf();
    in.close();
    const std::string &content = contents.str();

    bool ret = std::regex_search(content, match, std::regex(JSON_FIELD));
    if (match.empty() || !ret) {
        setError(Error::VersionInvalid);
        return false;
    }

    const std::string &vstr = match[1];

    ret = version.parse(vstr);
    if (!ret) {
        setError(Error::VersionInvalid);
        return false;
    }

    d->contents = content;
    setFilename(fileName);
    setVersion(version);

    return true;
}

bool JsonFile::save()
{
    std::string contents = d->contents;
    std::ofstream outfile(filename(), std::ofstream::out);

    if (!outfile) {
        setError(Error(Error::PathNotFound, filename()));
        return false;
    }

    const std::string vstr = "\"version\": \"" + version().str() + "\"";
    contents = std::regex_replace(contents, std::regex(JSON_FIELD), vstr,
                                  std::regex_constants::format_first_only);

    outfile << contents;

    outfile.close();

    return true;
}

std::vector<std::string> JsonFile::filenames() const
{
    return { "package.json" };
}

#if 1
bool JsonFile::detect(const std::string &dirname)
{
    std::error_code err;

    const bool ret = std::filesystem::exists(dirname, err);
    if (!ret) {
        setError(Error(Error::PathNotFound, dirname));
        return false;
    }

    for (auto filename : filenames()) {
        std::filesystem::path path(dirname);
        path /= filename;
        const bool ret = std::filesystem::exists(path, err);
        if (ret) {
            return readFile(path.string());
        }
    }

    return false;
}
#endif
