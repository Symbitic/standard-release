#include "standard-release/standard-release.h"
#include "standard-release/changelog/changelog.h"
#include "standard-release/commits/conventional.h"
#include "standard-release/config/yaml.h"
#include "standard-release/errors/error.h"
#include "standard-release/git/hooks.h"
#include "standard-release/git/repository.h"
#include "standard-release/semver/semver.h"
#include "standard-release/sources/json.h"
#include "standard-release/sources/text.h"
#include <fstream>
#include <iostream>

using namespace StandardRelease;

/*
https://github.com/googleapis/release-please
https://github.com/googleapis/release-please#release-types-supported
https://github.com/googleapis/release-please/blob/master/src/updaters/python/setup-cfg.ts
https://github.com/googleapis/release-please/blob/master/src/updaters/python/setup-py.ts
https://github.com/googleapis/release-please/blob/master/src/updaters/python/version-py.ts

*/

struct StandardRelease::MainPrivate
{
    GitRepository repo;
    Error error;
    IConventionalCommit *commits;
    ISource *versionFile;
    IConfig *config;
    IChangelog *changelog;
    Main::ReleaseType releaseType;
    std::string dirName;
    std::string configFile;
};

static ISource *scanForVersionFile(const std::string &dir)
{
    bool success = false;
    ISource *source = nullptr;

    source = new JsonFile();
    success = source->detect(dir);
    if (success) {
        return source;
    } else {
        delete source;
    }

    source = new TextFile();
    success = source->detect(dir);
    if (success) {
        return source;
    } else {
        delete source;
    }

    return nullptr;
}

Main::Main()
    : d(new MainPrivate)
{
    d->commits = new ConventionalCommits();
    d->repo = GitRepository();
}

Main::Main(const std::string &dirname)
    : Main()
{
    d->dirName = dirname;
}

Error Main::error() const
{
    return d->error;
}

std::string Main::directory() const
{
    return d->dirName;
}

void Main::setDirname(const std::string &dirName)
{
    d->dirName = dirName;
}

std::string Main::scanForConfigFile(const std::string &dirName)
{
    std::vector<std::string> fileNames { "release.yml", ".release.yml" };
    std::error_code err;

    const bool ret = std::filesystem::exists(dirName, err);
    if (!ret) {
        throw Exception(DIRECTORY_NOT_FOUND(dirName));
    }

    for (auto filename : fileNames) {
        std::filesystem::path path(dirName);
        path /= filename;
        const bool ret = std::filesystem::exists(path, err);
        if (ret) {
            return path.string();
        }
    }

    return "";
}

std::string Main::readFile(const std::string &fileName)
{
    std::ifstream in(fileName, std::ios::in | std::ios::binary);
    std::ostringstream contents;
    if (!in) {
        throw Exception(FILE_NOT_FOUND(fileName));
    }
    contents << in.rdbuf();
    in.close();
    return contents.str();
}

void Main::readConfigFile(const std::string &fileName)
{
    std::error_code code;
    std::string filename = fileName;

    if (filename.empty()) {
        filename = scanForConfigFile(d->dirName);
    }

    if (filename.empty()) {
        throw Exception("No config file found");
    }

    d->configFile = filename;

    const bool exists = std::filesystem::exists(filename, code);
    if (!exists) {
        throw Exception("Config file does not exist");
    }

    std::ifstream in(filename, std::ios::in | std::ios::binary);
    std::ostringstream contents;

    if (!in) {
        throw Exception("Error reading config file");
    }
    contents << in.rdbuf();
    in.close();
    const std::string &content = contents.str();

    d->config = new YamlConfig(filename);
    bool r = d->config->parse();
}

bool Main::init()
{
    return false;
}

bool Main::lint()
{
    return false;
}

////////////////////////////////////////////////////////////////////////////////

bool Main::release()
{
    d->repo.open(directory());
    if (d->repo.error()) {
        throw Exception(d->repo.error());
        return false;
    }

    auto releaseType = d->config->value("releaseType");
    ISource *versionFile = nullptr;

    if (releaseType == "node") {
        versionFile = new JsonFile();
    } else if (releaseType == "text") {
        // Not sure why it needs the namespace.
        versionFile = new StandardRelease::TextFile();
    } else {
        throw Exception("Unrecognized project type " + releaseType);
    }

    bool r = d->repo.open(directory());
    if (!r) {
        throw Exception("Not a git repository");
        return false;
    }

    versionFile->detect(directory());
    if (versionFile->error()) {
        throw Exception("Project version files not found");
    }

    // std::cout << versionFile->version() << std::endl;

    d->versionFile = versionFile;

    d->repo.parse(d->versionFile->version());

    auto repo_commits = d->repo.commits();

    // std::cout << d->repo.dirName() << std::endl;

    d->commits->setVersion(d->versionFile->version());
    d->commits->parseCommits(repo_commits);
    d->commits->bump();

    // std::cout << d->commits->version() << std::endl;

    const std::filesystem::path changelogPath = d->repo.dirName() / "CHANGELOG.md";

    d->changelog = new Changelog();
    d->changelog->setFilename(changelogPath);
    d->changelog->read();
    const auto oldVersion = d->versionFile->version();
    const auto newVersion = d->commits->version();
    d->changelog->generate(newVersion, oldVersion, d->commits->commits(), d->repo.url());

    d->versionFile->setVersion(d->commits->version());
    d->versionFile->save();
    d->changelog->write();

    //

    r = d->repo.createRelease(d->commits->version());

    /*
    auto commitMsg = std::string("chore(release): ") + d->commits->version().str();
    auto versionStr = "v" + d->commits->version().str();

    r = d->repo.commit(commitMsg);
    if (!r) {
        throw Exception("Error commiting changes");
    }

    r = d->repo.createTag(versionStr, commitMsg);
    if (!r) {
        throw Exception("Error creating tag");
    }

    d->repo.push();
    */

    return false;
}
