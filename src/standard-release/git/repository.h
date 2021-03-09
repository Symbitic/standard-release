#pragma once

#include "standard-release/errors/errors.h"
#include "standard-release/global/global.h"
#include <filesystem>
#include <list>
#include <string>
#include <vector>

// TODO: hide when GitRepoPrivate is implemented.
struct git_repository;

namespace StandardRelease {

/**
 * @brief Interface for a Git repository.
 */
class STANDARDRELEASE_EXPORT GitRepository
{
public:
    GitRepository();
    ~GitRepository();

    static bool isRepo(const std::string &dirName);

    struct Commit
    {
        std::string summary;
        std::string body;
        std::string hash;

        Commit(const char *str1, const char *str2 = "", const char *str3 = "")
            : summary(str1)
            , body(str2)
            , hash(str3) {};
    };

    using Commits = std::list<GitRepository::Commit>;

    /** Most recent error. */
    Error error() const;

    /** List of commits. */
    Commits commits() const;

    /** Remote origin URL. */
    std::string remoteUrl() const;

    /** Repository URL. */
    std::string url() const;

    /** Directory path. */
    std::filesystem::path dirName() const;

    /**
     * @brief Open a git repository.
     * @param[in] repo Path to a git repository.
     * @returns `true` if successful. Otherwise, `error()` will return an error description.
     */
    bool open(const std::filesystem::path &repo);

    bool commit(const std::string &msg);

    /**
     * @brief Push any changes and releases.
     * @returns `true` if successful. Otherwise, `error()` will return an error description.
     */
    bool push(const std::string &name);

    /**
     * @brief Create a release.
     * @param[in] version Release version.
     * @returns `true` if successful. Otherwise, `error()` will return an error description.
     */
    bool createRelease(const std::string &version);

    /**
     * @brief Parse an opened git repository.
     */
    bool parse(const std::string beginFrom);

    bool createTag(const std::string &name, const std::string msg);

private:
    bool parseRemotes();

    int countUnpushed();

    Error m_error;
    struct git_repository *m_repo;
    bool m_open;
    Commits m_commits;
    std::filesystem::path m_dirname;
    std::string m_remoteUrl;
    std::string m_url;
    std::vector<std::string> m_remotes;
};

}
