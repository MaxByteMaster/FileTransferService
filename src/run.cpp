#include <thread>
#include <unistd.h>

#include "process/Process.h"

int main()
{
    const auto server_process = Process::Create("Server");

    std::this_thread::sleep_for(std::chrono::seconds(2));

    const auto client_process = Process::Create("Client");

    return 0;
}
