#include <iostream>
#include <windows.h>
#include <vector>

#include "common.h"
#include "pipe_utils.h"
#include "locking.h"
#include "server_utils.h"

int main()
{
    std::string filename;
    std::cout << "Enter filename: ";
    std::cin >> filename;

    int n;
    std::cout << "Enter number of employees: ";
    std::cin >> n;

    std::vector<employee> database(n);

    createBinaryFile(filename, database); // из pipe_utils или file_utils
    printFile(filename);

    int clientCount;
    std::cout << "Enter number of clients: ";
    std::cin >> clientCount;

    startClients(clientCount); // CreateProcess()

    LockManager lockManager;

    for (int i = 0; i < clientCount; i++)
    {
        HANDLE hPipe = createServerPipe();
        ConnectNamedPipe(hPipe, nullptr);

        CreateThread(
            nullptr,
            0,
            [](LPVOID param) -> DWORD {
                PipeContext* ctx = static_cast<PipeContext*>(param);
                handleClient(ctx->hPipe, ctx->database, ctx->lockManager);
                delete ctx;
                return 0;
            },
            new PipeContext{ hPipe, database, lockManager },
            0,
            nullptr
        );
    }

    system("pause");

    printFile(filename);

    return 0;
}
