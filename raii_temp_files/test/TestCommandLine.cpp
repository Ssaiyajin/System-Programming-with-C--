#include "lib/CommandLine.hpp"
#include <chrono>
#include <climits>
#include <ftw.h>
#include <gtest/gtest.h>
#include <poll.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
//---------------------------------------------------------------------------
using namespace std;
using namespace raii;
//---------------------------------------------------------------------------
namespace {
//---------------------------------------------------------------------------
/// Clean the test directory before and after a test runs
class CleanTestDirectory {
    private:
    /// The directory
    string directory;

    /// Remove the test directory
    void clean() {
        struct stat buf;
        if (stat(directory.c_str(), &buf) != 0) {
            if ((errno == ENOENT) || (errno == ENOTDIR))
                return;

            perror("stat failed");
            cerr << "error while cleaning " << directory << ", please delete the directory manually" << endl;
            throw;
        }

        auto callback = [](const char* path, const struct stat* /*stat*/, int info, struct FTW * /*ftw*/) -> int {
            switch (info) {
                case FTW_D:
                    break;
                case FTW_NS:
                case FTW_DNR:
                    return 1;
                case FTW_DP:
                    if (rmdir(path) < 0) {
                        perror("rmdir failed");
                        return 1;
                    }

                    break;
                case FTW_F:
                case FTW_SL:
                case FTW_SLN:
                    if (unlink(path) < 0) {
                        perror("unlink failed");
                        return 1;
                    }

                    break;
            }

            return 0;
        };

        if (nftw(directory.c_str(), callback, 16, FTW_DEPTH | FTW_MOUNT | FTW_PHYS) != 0) {
            cerr << "error while cleaning " << directory << ", please delete the directory manually" << endl;
            throw;
        }
    }

    public:
    /// Constructor
    explicit CleanTestDirectory(string directory)
        : directory(move(directory)) {
        clean();
    }

    /// Destructor
    ~CleanTestDirectory() { clean(); }
};
//---------------------------------------------------------------------------
/// An inotify context
struct InotifyContext {
    /// The inotify fd
    int fd;

    /// Constructor
    InotifyContext() {
        if ((fd = inotify_init()) < 0) {
            perror("inotify_init failed");
            throw;
        }
    }

    /// Destructor
    ~InotifyContext() {
        close(fd);
    }
};
//---------------------------------------------------------------------------
/// Watch a directory for certain events
class DirectoryWatch {
    private:
    /// The inotify context
    const InotifyContext& context;
    /// The directory
    string directory;
    /// The watch descriptor
    int wd;

    /// Check if a certain file or directory has been created in the directory
    bool expectCreated(const string& filename, bool expectFile) {
        pollfd fds[1];
        fds[0].fd = context.fd;
        fds[0].events = POLLIN;

        int ret = poll(fds, 1, 500);

        if (ret < 0) {
            perror("poll failed");

            return false;
        } else if (ret == 0) {
            cerr << "no events occurred in directory " << directory << " after 500ms" << endl;

            return false;
        }

        vector<char> buffer(sizeof(inotify_event) + NAME_MAX + 1);
        if (read(context.fd, &buffer[0], buffer.size()) < 0) {
            perror("read failed");

            return false;
        }

        auto* event = reinterpret_cast<struct inotify_event*>(&buffer[0]);

        if (event->wd != wd) {
            cerr << "unexpected event" << endl;

            return false;
        }

        if (filename != string(event->name)) {
            cerr << "name mismatch (expected: " << filename << ", actual: " << string(event->name) << ")" << endl;
            return false;
        }

        if (!(event->mask & IN_CREATE)) {
            cerr << "event mismatch (expected: creation)" << endl;
            return false;
        }

        string fullPath = directory + "/" + filename;
        struct stat buf;
        if (stat(fullPath.c_str(), &buf) < 0) {
            perror("stat failed");

            return false;
        }

        if (expectFile) {
            if (!S_ISREG(buf.st_mode)) {
                cerr << "file type mismatch (expected file)" << endl;

                return false;
            }
        } else if (!S_ISDIR(buf.st_mode)) {
            cerr << "file type mismatch (expected directory)" << endl;

            return false;
        }

        return true;
    }

    public:
    /// Constructor
    DirectoryWatch(InotifyContext& context, string directory)
        : context(context),
          directory(move(directory)) {
        if ((wd = inotify_add_watch(context.fd, this->directory.c_str(), IN_CREATE | IN_DELETE)) < 0) {
            perror("inotify_add_watch failed");
            throw;
        }
    }

    /// Destructor
    ~DirectoryWatch() {
        if (inotify_rm_watch(context.fd, wd) < 0) {
            perror("inotify_rm_watch failed");
        } else {
            // consume the IN_IGNORE event generated by inotify_rm_watch
            vector<char> buffer(sizeof(inotify_event) + NAME_MAX + 1);
            if (read(context.fd, &buffer[0], buffer.size()) < 0)
                perror("read failed");
        }
    }

    /// Check if a certain file has been created in the directory
    bool expectFileCreated(const string& filename) {
        return expectCreated(filename, true);
    }

    /// Check if a certain directory has been created in the directory
    bool expectDirectoryCreated(const string& name) {
        return expectCreated(name, false);
    }

    /// Check if a certain file or directory has been deleted in the directory
    bool expectDeleted(const string& name) {
        pollfd fds[1];
        fds[0].fd = context.fd;
        fds[0].events = POLLIN;

        int ret = poll(fds, 1, 500);

        if (ret < 0) {
            perror("poll failed");

            return false;
        } else if (ret == 0) {
            cerr << "no events occurred in directory " << directory << " after 500ms" << endl;

            return false;
        }

        vector<char> buffer(sizeof(inotify_event) + NAME_MAX + 1);
        if (read(context.fd, &buffer[0], buffer.size()) < 0) {
            perror("read failed");

            return false;
        }

        auto* event = reinterpret_cast<struct inotify_event*>(&buffer[0]);

        if (name != string(event->name)) {
            cerr << "name mismatch (expected: " << name << ", actual: " << string(event->name) << ")" << endl;
            return false;
        }

        if (!(event->mask & IN_DELETE)) {
            cerr << "event mismatch (expected: deletion)" << endl;
            return false;
        }

        return true;
    }
};
//---------------------------------------------------------------------------
/// Tester for command line interface
class Tester {
    private:
    /// Process ID of the child
    int pid = -1;
    /// Pipe used for communication from parent to child (output of pipe is redirected to stdin of child)
    int parentToChild[2];
    /// Pipe used for communication from parent to child (signal parent ready)
    int parentReady[2];

    public:
    /// Constructor
    Tester() {
        if (pipe(parentToChild) != 0) {
            perror("pipe failed");
            throw;
        }
    }

    /// Destructor
    ~Tester() {
        if (isParent()) {
            sigset_t set;
            sigemptyset(&set);
            sigaddset(&set, SIGCHLD);

            auto remaining = 0.5;
            auto begin = chrono::high_resolution_clock::now();

            bool childExited = false;
            while (!childExited && (remaining > 0)) {
                struct timespec timeout;
                timeout.tv_sec = 0;
                timeout.tv_nsec = static_cast<long>(1e9 * remaining); // remaining time in ns

                siginfo_t info;
                if ((sigtimedwait(&set, &info, &timeout) == SIGCHLD) && (info.si_code == CLD_EXITED))
                    childExited = true;

                auto elapsed = (chrono::high_resolution_clock::now() - begin).count();
                remaining = 0.5 - elapsed;
            }

            if (!childExited) {
                cerr << "child did not exit after 500ms, sending SIGKILL" << endl;
                ADD_FAILURE();

                kill(pid, SIGKILL);
            }

            waitpid(pid, nullptr, 0);
        }

        close(parentToChild[1]);
    }

    /// Check if the current process is the parent process
    bool isParent() const { return pid != 0; }

    /// Setup the tester
    void setup() {
        int childReady[2];
        if (pipe(childReady) < 0) {
            perror("pipe failed");
            throw;
        }

        if (pipe(parentReady) < 0) {
            perror("pipe failed");
            throw;
        }

        sigset_t set;
        sigemptyset(&set);
        sigaddset(&set, SIGCHLD);
        sigprocmask(SIG_BLOCK, &set, nullptr);

        if ((pid = fork()) < 0) {
            perror("fork failed");
            throw;
        }

        if (!isParent()) {
            // Redirect stdin
            if (dup2(parentToChild[0], STDIN_FILENO) < 0) {
                perror("dup2 failed");
                throw;
            }

            bool ready = true;
            if (::write(childReady[1], &ready, sizeof(ready)) != sizeof(ready)) {
                perror("write failed");
                throw;
            }

            if (!ready) {
                cerr << "child signaled initialization error" << endl;
                throw;
            }

            // close unneeded fds
            close(parentToChild[0]);
            close(childReady[0]);
            close(childReady[1]);
        } else {
            // wait for child initialization
            pollfd fds[1];
            fds[0].fd = childReady[0];
            fds[0].events = POLLIN;

            int ret = poll(fds, 1, 500);

            if (ret < 0) {
                perror("poll failed");
                throw;
            } else if (ret == 0) {
                cerr << "child was not ready within 500ms" << endl;
                throw;
            }

            bool ready;
            if (::read(childReady[0], &ready, sizeof(ready)) != sizeof(ready)) {
                perror("read failed");
                throw;
            }

            // close unneeded fds
            close(parentToChild[0]);
            close(childReady[0]);
            close(childReady[1]);
        }
    }

    /// Run the tester
    void run(const string& directory) {
        if (!isParent()) {
            bool ready;
            if (::read(parentReady[0], &ready, sizeof(ready)) != sizeof(ready)) {
                perror("read failed");
                throw;
            }

            // start cli
            CommandLine cli;
            cli.run(directory);
        } else {
            bool ready = true;
            if (::write(parentReady[1], &ready, sizeof(ready)) != sizeof(ready)) {
                perror("write failed");
                throw;
            }

            close(parentReady[0]);
            close(parentReady[1]);
        }
    }

    void write(const string& command) {
        if (::write(parentToChild[1], command.c_str(), command.size()) < 0) {
            perror("write failed");
            throw;
        }
    }
};
//---------------------------------------------------------------------------
} // namespace
//---------------------------------------------------------------------------
TEST(TestCommandLine, createFilesAndQuit) {
    CleanTestDirectory clean("/tmp/raii_test");

    Tester tester;
    tester.setup();

    if (tester.isParent()) {
        InotifyContext context;

        {
            DirectoryWatch watch(context, "/tmp");
            tester.run("/tmp/raii_test");

            ASSERT_TRUE(watch.expectDirectoryCreated("raii_test"));
        }

        {
            DirectoryWatch watch(context, "/tmp/raii_test");
            tester.write("enter\n");
            ASSERT_TRUE(watch.expectDirectoryCreated("dir0"));
        }

        {
            DirectoryWatch watch(context, "/tmp/raii_test/dir0");
            tester.write("create\n");
            ASSERT_TRUE(watch.expectFileCreated("file0"));

            tester.write("create\n");
            ASSERT_TRUE(watch.expectFileCreated("file1"));
        }

        tester.write("leave\n");

        {
            DirectoryWatch watch(context, "/tmp/raii_test");
            tester.write("create\n");
            ASSERT_TRUE(watch.expectFileCreated("file2"));

            tester.write("enter\n");
            ASSERT_TRUE(watch.expectDirectoryCreated("dir1"));
        }

        {
            DirectoryWatch watch(context, "/tmp");
            tester.write("quit\n");
            ASSERT_TRUE(watch.expectDeleted("raii_test"));
        }
    } else {
        tester.run("/tmp/raii_test");
        exit(0); // otherwise gtest is unhappy
    }
}
//---------------------------------------------------------------------------
TEST(TestCommandLine, leaveEmptyDirectory) {
    CleanTestDirectory clean("/tmp/raii_test");

    Tester tester;
    tester.setup();

    if (tester.isParent()) {
        InotifyContext context;

        {
            DirectoryWatch watch(context, "/tmp");
            tester.run("/tmp/raii_test");
            ASSERT_TRUE(watch.expectDirectoryCreated("raii_test"));
        }

        {
            DirectoryWatch watch(context, "/tmp/raii_test");
            tester.write("enter\n");
            ASSERT_TRUE(watch.expectDirectoryCreated("dir0"));

            tester.write("leave\n");
            ASSERT_TRUE(watch.expectDeleted("dir0"));
        }

        {
            DirectoryWatch watch(context, "/tmp");
            tester.write("quit\n");
            ASSERT_TRUE(watch.expectDeleted("raii_test"));
        }
    } else {
        tester.run("/tmp/raii_test");
        exit(0); // otherwise gtest is unhappy
    }
}
//---------------------------------------------------------------------------
TEST(TestCommandLine, createAndRemoveFiles) {
    CleanTestDirectory clean("/tmp/raii_test");

    Tester tester;
    tester.setup();

    if (tester.isParent()) {
        InotifyContext context;

        {
            DirectoryWatch watch(context, "/tmp");
            tester.run("/tmp/raii_test");
            ASSERT_TRUE(watch.expectDirectoryCreated("raii_test"));
        }

        {
            DirectoryWatch watch(context, "/tmp/raii_test");
            tester.write("enter\n");
            ASSERT_TRUE(watch.expectDirectoryCreated("dir0"));
        }

        {
            DirectoryWatch watch(context, "/tmp/raii_test/dir0");
            tester.write("create\n");
            ASSERT_TRUE(watch.expectFileCreated("file0"));

            tester.write("create\n");
            ASSERT_TRUE(watch.expectFileCreated("file1"));
        }

        tester.write("leave\n");

        {
            DirectoryWatch watch(context, "/tmp/raii_test/dir0");
            tester.write("remove 0\n");
            ASSERT_TRUE(watch.expectDeleted("file0"));
        }

        {
            DirectoryWatch watch(context, "/tmp/raii_test");
            tester.write("remove 0\n");
            ASSERT_TRUE(watch.expectDeleted("dir0"));
        }

        {
            DirectoryWatch watch(context, "/tmp");
            tester.write("quit\n");
            ASSERT_TRUE(watch.expectDeleted("raii_test"));
        }
    } else {
        tester.run("/tmp/raii_test");
        exit(0); // otherwise gtest is unhappy
    }
}
//---------------------------------------------------------------------------
