#ifndef PROCESS_H
#define PROCESS_H

#include <memory>

class Process
{
    using ProcessUPtr = std::unique_ptr<Process>;

public:
    static ProcessUPtr Create(char const* filename);
    ~Process();

private:
    pid_t id_;

    Process();
};

#endif // PROCESS_H
