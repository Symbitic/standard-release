#include "text.h"
#include "standard-release/semver/semver.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <system_error>

using namespace StandardRelease;

struct StandardRelease::TextFilePrivate
{
    std::string contents;
    size_t len;
    size_t pos;
};

TextFile::TextFile()
    : ISource()
    , d(new TextFilePrivate) {};

bool TextFile::readFile(const std::string &filename)
{
    std::ifstream in(filename, std::ios::in | std::ios::binary);
    std::ostringstream contents;
    std::smatch match;
    SemVer version;

    if (!in) {
        setError(Error(Error::PathNotFound, filename));
        return false;
    }
    contents << in.rdbuf();
    in.close();
    const std::string &content = contents.str();

    std::regex_search(content, match, std::regex(VERSION));
    if (match.empty()) {
        setError(Error::VersionInvalid);
        return false;
    }
    const std::string &vstr = content.substr(match.position(), match.length());

    bool ret = version.parse(vstr);
    if (!ret) {
        setError(Error::VersionInvalid);
        return false;
    }

    d->contents = content;
    d->pos = match.position();
    d->len = match.length();
    setFilename(filename);
    setVersion(version);

    return true;
}

bool TextFile::save()
{
    std::string contents = d->contents;
    std::ofstream outfile(filename(), std::ofstream::out);

    if (!outfile) {
        setError(Error(Error::PathNotFound, filename()));
        return false;
    }

    contents.replace(d->pos, d->len, version());
    outfile << contents;

    outfile.close();

    return false;
}

std::vector<std::string> TextFile::filenames() const
{
    return { "VERSION", "VERSION.txt" };
}

bool TextFile::detect(const std::string &dirname)
{
    std::vector<std::string> fileNames { "VERSION", "VERSION.txt" };
    std::error_code err;

    const bool ret = std::filesystem::exists(dirname, err);
    if (!ret) {
        setError(Error(Error::PathNotFound, dirname));
        return false;
    }

    for (auto filename : fileNames) {
        std::filesystem::path path(dirname);
        path /= filename;
        const bool ret = std::filesystem::exists(path, err);
        if (ret) {
            return readFile(path.string());
        }
    }

    return false;
}
