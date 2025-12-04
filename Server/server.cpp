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

    createBinaryFile(filename, database);
    printFile(filename);

    int clientCount;
    std::cout << "Enter number of clients: ";
    std::cin >> clientCount;

    startClients(clientCount);

    LockManager lockManager;
    std::vector<HANDLE> clientThreads;

    for (int i = 0; i < clientCount; i++)
    {
        HANDLE hPipe = createServerPipe();

        if (ConnectNamedPipe(hPipe, nullptr) || GetLastError() == ERROR_PIPE_CONNECTED)
        {
            std::cout << "Client " << i + 1 << " connected.\n";

            PipeContext* ctx = new PipeContext{ hPipe, database, &lockManager };

            HANDLE hThread = CreateThread(
                nullptr,
                0,
                [](LPVOID param) -> DWORD {
                    PipeContext* ctx = static_cast<PipeContext*>(param);
                    handleClient(ctx->hPipe, ctx->database, *ctx->lockManager);
                    delete ctx;
                    return 0;
                },
                ctx,
                0,
                nullptr
            );

            if (hThread)
                clientThreads.push_back(hThread);
        }
        else
        {
            CloseHandle(hPipe);
        }
    }

    std::cout << "Processing clients... Waiting for them to disconnect.\n";
    if (!clientThreads.empty()) {
        WaitForMultipleObjects(clientThreads.size(), clientThreads.data(), TRUE, INFINITE);
    }

    for (HANDLE h : clientThreads) CloseHandle(h);

    std::cout << "All clients finished.\n";

    saveFile(filename, database);
    printFile(filename);

    std::cout << "Press Enter to exit server...";
    std::cin.ignore();
    std::cin.get();

    return 0;
}