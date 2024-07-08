#include "Process.h"

#include <cstring>
#include <iostream>
#include <linux/limits.h>
#include <unistd.h>
#include <vector>

Process::ProcessUPtr Process::Create(char const* filename)
{
    auto instance = ProcessUPtr(new Process());
    if (instance->id_ == -1)
    {
        std::cerr << "Failed to fork process: " << strerror(errno) << std::endl;
        return nullptr;
    }

    if (instance->id_ != 0)
        return instance;

    char absolute_path[PATH_MAX];
    if (realpath(filename, absolute_path) == NULL)
    {
        std::cerr << "Error resolving the absolute path of [" << filename << "]: " << strerror(errno) << std::endl;
        return nullptr;
    }

    /* It is a bad solution, but for unknown reason I cannot use pipes
    for redirecting output of a processes */
    std::vector<const char*> terminals = {
        "x-terminal-emulator",
        "gnome-terminal",
        "konsole",
        "xfce4-terminal",
        "xterm"
    };

    for (const auto& terminal : terminals)
        execlp(terminal, terminal, "-e", absolute_path, NULL);

    // Process will return here only in case of failure of execlp
    std::cerr << "[" << filename << "]" << "Error when executing execlp: " << strerror(errno) << std::endl;
    exit(1);
}

Process::~Process()
{

}

Process::Process()
{
    id_ = fork();
}
