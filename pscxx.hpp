#ifndef PSCXX_HPP
#define PSCXX_HPP

#include <cstdlib>
#include <cstring>
#include <string>
#include <sys/poll.h>
#include <sys/wait.h>
#include <vector>
#include <optional>
#include <unistd.h>
#include <cerrno>
#include <spawn.h>
#include <poll.h>

extern char **environ;

namespace pscxx

{

struct PRESULT
{
    int exitCode;
    int signal;
    std::string outStd;
    std::string errStd;
};

inline PRESULT execute(std::vector<std::string> execution_argv, std::string binary_path, std::optional<char**> environment = environ)
{
    std::vector<char*> argv;

    try
    {
        for (const auto& arg : execution_argv)
        {
            argv.push_back(const_cast<char*>(arg.c_str()));
        }
        argv.push_back(nullptr);
    }
    catch (const std::bad_optional_access& e)
    {
        return (PRESULT){1, 0, "", e.what()};
    }

    int pipe_stdout[2];
    int pipe_stderr[2];
    if (pipe(pipe_stdout) == -1)
    {
        return (PRESULT){EPIPE, 0, "", std::strerror(EPIPE)};
    }
    if (pipe(pipe_stderr) == -1)
    {
        return (PRESULT){EPIPE, 0, "", std::strerror(EPIPE)};
    }

    posix_spawn_file_actions_t action;
    posix_spawn_file_actions_init(&action);

    posix_spawn_file_actions_adddup2(&action, pipe_stdout[1], STDOUT_FILENO);
    posix_spawn_file_actions_adddup2(&action, pipe_stderr[1], STDERR_FILENO);

    posix_spawn_file_actions_addclose(&action, pipe_stdout[0]);
    posix_spawn_file_actions_addclose(&action, pipe_stderr[0]);
    posix_spawn_file_actions_addclose(&action, pipe_stdout[1]);
    posix_spawn_file_actions_addclose(&action, pipe_stderr[1]);

    pid_t pid;

    int statusCode = posix_spawn(&pid, binary_path.c_str(), &action, NULL, argv.data(), static_cast<char* const*>(*environment));

    posix_spawn_file_actions_destroy(&action);
    close(pipe_stdout[1]);
    close(pipe_stderr[1]);

    if (statusCode != 0)
    {
        close(pipe_stdout[0]);
        close(pipe_stderr[0]);
        return (PRESULT){statusCode, 0, "", std::strerror(statusCode)};
    }

    int statusWait;

    std::string out;
    std::string err;
    char buff_out[256];
    char buff_err[256];
    ssize_t bytes_read_out;
    ssize_t bytes_read_err;

    struct pollfd fd_polls[2];

    fd_polls[0].fd = pipe_stdout[0];
    fd_polls[1].fd = pipe_stderr[0];

    for (int i = 0; i < 2; i++)
    {
        fd_polls[i].events = POLLIN;
        fd_polls[i].revents = 0;
    }

    bool stdout_active = true;
    bool stderr_active = true;

    while (stdout_active || stderr_active)
    {
        int poll_returns = poll(fd_polls, 2, -1);

        for (int i = 0; i < 2; i++)
        {
            if (fd_polls[i].revents & (POLLIN | POLLHUP | POLLERR) && fd_polls[i].fd == pipe_stdout[0] && stdout_active)
            {
                if ((bytes_read_out = read(pipe_stdout[0], buff_out, sizeof(buff_out)) - 1) > 0)
                {
                    buff_out[bytes_read_out] = '\0';
                    out.append(buff_out);
                }
                else
                {
                    fd_polls[i].fd = -1;
                    stdout_active = false; 
                }
            }

            if (fd_polls[i].revents & (POLLIN | POLLHUP | POLLERR) && fd_polls[i].fd == pipe_stderr[0] && stderr_active)
            {
                if ((bytes_read_err = read(pipe_stderr[0], buff_err, sizeof(buff_err) - 1)) > 0)
                {
                    buff_err[bytes_read_err] = '\0';
                    err.append(buff_err);
                }
                else
                {
                    fd_polls[i].fd = -1;
                    stderr_active = false; 
                }
            }
        }
    }

    close(pipe_stdout[0]);
    close(pipe_stderr[0]);

    waitpid(pid, &statusWait, 0);

    if (WIFEXITED(statusWait))
    {
        return (PRESULT){WEXITSTATUS(statusWait), 0, out, err};
    }
    else if (WIFSIGNALED(statusWait))
    {
        int sig = WTERMSIG(statusWait);

        return (PRESULT){-1, sig, out, err};
    }
    else
    {
        return (PRESULT){-1, WSTOPSIG(statusWait), out, err};
    }
}

}

#endif //PSCXX_HPP