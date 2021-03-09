#include "repository.h"
#include "git2/branch.h"
#include "git2/commit.h"
#include "git2/errors.h"
#include "git2/global.h"
#include "git2/graph.h"
#include "git2/index.h"
#include "git2/refs.h"
#include "git2/remote.h"
#include "git2/repository.h"
#include "git2/revparse.h"
#include "git2/revwalk.h"
#include "git2/signature.h"
#include "git2/status.h"
#include "git2/tag.h"
#include "standard-release/errors/error.h"
#include <filesystem>
#include <iostream>
#include <regex>
#include <system_error>

using namespace StandardRelease;

static const char *git2error()
{
    const git_error *err = git_error_last();
    if (err != NULL && err->message != NULL) {
        return err->message;
    }

    return "";
}

GitRepository::GitRepository()
    : m_error()
    , m_repo()
    , m_open(false)
    , m_commits()
    , m_remoteUrl()
    , m_url()
{
    git_libgit2_init();
}

GitRepository::~GitRepository()
{
    if (m_repo != nullptr) {
        git_repository_free(m_repo);
    }
    git_libgit2_shutdown();
    m_open = false;
}

bool GitRepository::isRepo(const std::string &dirName)
{
    std::error_code code;
    const auto dir = std::filesystem::path(dirName) / ".git";
    const bool exists = std::filesystem::exists(dir, code);
    return exists;
}

Error GitRepository::error() const
{
    return m_error;
}

std::list<GitRepository::Commit> GitRepository::commits() const
{
    return m_commits;
}

std::filesystem::path GitRepository::dirName() const
{
    return m_dirname;
}

std::string GitRepository::remoteUrl() const
{
    return m_remoteUrl;
}

std::string GitRepository::url() const
{
    return m_url;
}

int GitRepository::countUnpushed()
{
    int ret;
    size_t ahead;
    size_t behind;
    git_reference *local_ref = nullptr;
    git_reference *remote_ref = nullptr;

    ret = git_repository_head(&local_ref, m_repo);
    if (ret != GIT_OK) {
        m_error = Error(Error::GitInvalidRepo, git2error());
        return 0;
    }

    ret = git_branch_upstream(&remote_ref, local_ref);
    if (ret != GIT_OK) {
        m_error = Error(Error::GitInvalidRepo, git2error());
        return 0;
    }

    const git_oid *local_oid = git_reference_target(local_ref);
    const git_oid *remote_oid = git_reference_target(remote_ref);
    if (local_oid == nullptr || remote_oid == nullptr) {
        m_error = Error(Error::GitInvalidRepo, "Unable to resolve remote");
        return 0;
    }

    ret = git_graph_ahead_behind(&ahead, &behind, m_repo, local_oid, remote_oid);
    if (ret != GIT_OK) {
        m_error = Error(Error::GitInvalidRepo, git2error());
        return 0;
    }

    git_reference_free(local_ref);
    git_reference_free(remote_ref);

    return ahead;
}

bool GitRepository::push(const std::string &name)
{
    int ret;
    git_reference *head = nullptr;
    git_push_options options;
    git_remote *remote = nullptr;
    const char *remoteName = "origin"; // TODO: Do not hardcode origin.

    // ret = git_repository_head(&head, m_repo);
    ret = git_reference_lookup(&head, m_repo, name.c_str());
    if (ret != GIT_OK) {
        m_error = Error(Error::GitInvalidRepo, git2error());
        return false;
    }

    const char *refspec = git_reference_name(head);
    const git_strarray refspecs = { (char **)&refspec, 1 };

    const int ahead = countUnpushed();
    if (ahead != 1) {
        // throw Exception('')
    }

    git_reference_free(head);

    // std::cout << "Ahead by " << countUnpushed() << std::endl;
    // std::cout << "refspec = " << refspec << "\n";

    ret = git_remote_lookup(&remote, m_repo, remoteName);
    if (ret != GIT_OK) {
        throw Exception(git2error());
        m_error = Error(Error::InternalError, git2error());
        return false;
    }

    ret = git_push_options_init(&options, GIT_PUSH_OPTIONS_VERSION);
    if (ret != GIT_OK) {
        throw Exception(git2error());
        m_error = Error(Error::InternalError, git2error());
        return false;
    }

    ret = git_remote_push(remote, &refspecs, &options);
    if (ret != GIT_OK) {
        throw Exception(git2error());
        m_error = Error(Error::InternalError, git2error());
        return false;
    }

    return true;
}

bool GitRepository::createRelease(const std::string &version)
{
    bool r;
    auto commitMsg = std::string("chore(release): ") + version;
    auto versionStr = "v" + version;

    r = commit(commitMsg);
    if (!r) {
        throw Exception("Error commiting changes");
    }

    r = createTag(versionStr, commitMsg);
    if (!r) {
        throw Exception("Error creating tag");
    }

    if (!remoteUrl().empty()) {
        const std::string headRef = "refs/heads/master"; // "HEAD";
        const std::string tagRef = "refs/tags/" + versionStr;

        r = push(headRef);
        if (!r) {
            throw Exception("Error pushing to origin");
        }

        r = push(tagRef);
        if (!r) {
            throw Exception("Error pushing tag");
        }
    }

    // Not working yet.
    return true;
}

bool GitRepository::open(const std::filesystem::path &repo)
{
    std::error_code code;
    int ret;
    bool result;

    if (repo.empty()) {
        m_error = Error(Error::EmptyPath);
        return false;
    }

    const bool exists = std::filesystem::exists(repo, code);
    if (!exists) {
        m_error = Error(Error::PathNotFound, repo.string());
        return false;
    }

    ret = git_repository_open(&m_repo, repo.string().c_str());
    if (ret != GIT_OK) {
        m_error = Error(Error::ErrorOpeningGitRepo, git2error());
        return false;
    }

    m_open = true;
    m_dirname = repo;

    return true;
}

bool GitRepository::createTag(const std::string &name, const std::string msg)
{
    int ret;
    std::stringstream estr;
    git_signature *sig = nullptr;
    git_oid oid;
    git_object *obj = nullptr;
    const char *target = "HEAD";
    const int force = 0;

    ret = git_revparse_single(&obj, m_repo, target);
    if (ret != GIT_OK) {
        m_error = Error(Error::GitInvalidSpec, git2error());
        return false;
    }

    ret = git_signature_default(&sig, m_repo);
    if (ret != GIT_OK) {
        m_error = Error(Error::GitBadSignature, git2error());
        return false;
    }

    ret = git_tag_create(&oid, m_repo, name.c_str(), obj, sig, msg.c_str(), force);
    if (ret != GIT_OK) {
        m_error = Error(Error::ErrorCreatingTag, git2error());
        return false;
    }

    git_object_free(obj);
    git_signature_free(sig);

    // Not done?
    // return false;

    return true;
}

bool GitRepository::parseRemotes()
{
    int ret;
    git_strarray remotes;
    std::stringstream estr;

    ret = git_remote_list(&remotes, m_repo);
    if (ret != GIT_OK) {
        m_error = Error(Error::GitNoRemotes);
        return false;
    }

    for (int i = 0; i < remotes.count; i++) {
        git_remote *remote = nullptr;

        ret = git_remote_lookup(&remote, m_repo, remotes.strings[i]);
        if (ret != GIT_OK) {
            estr << "Error looking up remote '" << remotes.strings[i] << "' - " << git2error();
            // m_error = estr.str();
            return false;
        }

        const char *url = git_remote_url(remote);
        const char *name = git_remote_name(remote);

        if (std::string(url).find("github") != std::string::npos) {
            // std::cout << "Found GitHub\n";
        }

        m_remotes.push_back(git_remote_name(remote));

        // std::cout << git_remote_name(remote) << std::endl;

        git_remote_free(remote);
    }

    git_strarray_free(&remotes);

    return true;
}

bool GitRepository::parse(const std::string beginFrom)
{
    int ret;
    std::stringstream range;
    const std::string remoteRegex = "git@([^:]+):(.*)(?=.git|$)";
    std::smatch match;
    git_oid oid;
    git_revwalk *walker = nullptr;
    git_object *rev = nullptr;
    git_remote *remote = nullptr;
    std::string fromStr = beginFrom;
    fromStr.insert(0, 1, 'v');

    if (!m_open) {
        m_error = Error(Error::InternalError, "parse() called before repo was opened");
        return false;
    }

    ret = git_revwalk_new(&walker, m_repo);
    if (ret != GIT_OK) {
        m_error = Error(Error::InternalError, "error traversing git repo");
        return false;
    }

    git_revwalk_sorting(walker, GIT_SORT_NONE);

    ret = git_revparse_single(&rev, m_repo, fromStr.c_str());
    if (ret != GIT_OK) {
        // Parse from the beginning.
        ret = git_revparse_single(&rev, m_repo, "HEAD");
        if (ret == GIT_OK) {
            fromStr = "";
        }
    }
    git_object_free(rev);

    // Keep for when starting from beginning.
    git_revwalk_push_head(walker);

    if (!fromStr.empty()) {
        const char *from = fromStr.c_str();
        const char *to = "HEAD";
        range << from << ".." << to;

        ret = git_revwalk_push_range(walker, range.str().c_str());
        if (ret != GIT_OK) {
            m_error = Error(Error::InternalError,
                            "invalid git range requested (" + range.str() + ")");
            return false;
        }
    }

    while (git_revwalk_next(&oid, walker) == GIT_OK) {
        git_commit *commit = nullptr;
        char sha1[8] = { 0 };

        git_commit_lookup(&commit, m_repo, &oid);
        git_oid_tostr(sha1, sizeof(sha1), &oid);

        const char *summary = git_commit_summary(commit);
        const char *body = git_commit_body(commit);

        if (body == nullptr) {
            m_commits.push_back(Commit(summary, "", sha1));
        } else {
            m_commits.push_back(Commit(summary, body, sha1));
        }

        git_commit_free(commit);
    }

    git_revwalk_free(walker);

    parseRemotes();

    // TODO: Do not hardcode origin.
    ret = git_remote_lookup(&remote, m_repo, "origin");
    if (ret != GIT_OK) {
        m_error = Error(Error::InternalError, git2error());
        return false;
    }

    m_remoteUrl = git_remote_url(remote);
    // const char *name = git_remote_name(remote);

    {
        // Extract baseUrl.
        std::regex_search(m_remoteUrl, match, std::regex(remoteRegex));
        if (match.empty()) {
            // setError(Error::InternalError);
            return false;
        }

        std::string host = match[1];
        std::string repo = match[2];

        if (repo.substr(repo.length() - 4, 4) == ".git") {
            repo = repo.substr(0, repo.length() - 4);
        }

        m_url = "https://" + host + "/" + repo;
    }

    git_remote_free(remote);

    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool GitRepository::commit(const std::string &msg)
{
    int ret = 0;
    git_object *parent = nullptr;
    git_reference *ref = nullptr;
    git_index *index = nullptr;
    git_oid tree_oid;
    git_oid commit_oid;
    git_tree *tree = nullptr;
    git_signature *signature = nullptr;
    const char *glob = ".";
    const git_strarray pathspec = { (char **)&glob, 1 };

    auto cb = [](const char *path, const char *matched_pathspec, void *payload) {
        // Add all changed files.
        return 0;
    };

    ret = git_revparse_ext(&parent, &ref, m_repo, "HEAD");
    ret = git_repository_index(&index, m_repo);
    ret = git_index_add_all(index, &pathspec, GIT_INDEX_ADD_DEFAULT, cb, nullptr);
    ret = git_index_write_tree(&tree_oid, index);
    git_index_write(index);

    ret = git_tree_lookup(&tree, m_repo, &tree_oid);

    ret = git_signature_default(&signature, m_repo);
    ret = git_commit_create_v(&commit_oid, m_repo, "HEAD", signature, signature, nullptr,
                              msg.c_str(), tree, 1, parent);
    if (ret != GIT_OK) {
        throw Exception("Error commiting release");
    }

    git_index_free(index);
    git_signature_free(signature);
    git_tree_free(tree);

    return true;
}
