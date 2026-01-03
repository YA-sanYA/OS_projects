//#include <iostream>
//#include <vector>
//#include <thread>
//#include <mutex>
//#include <condition_variable>
//#include <random>
//#include <chrono>
//#include <stdexcept>
//#include "utils.h"
//#include "Threads.h"
//
//int main() {
//    try {
//        int arraySize = readNaturalNumber("Enter array size: ", "Invalid array size");
//        std::vector<int> array(arraySize, 0);
//
//        int threadCount = readNaturalNumber("Enter number of threads: ", "Invalid thread number");
//        std::mutex arrMutex;
//
//        std::vector<std::thread> threads;
//        std::vector<std::unique_ptr<ThreadDataForThreads>> threadData;
//
//        for (int i = 0; i < threadCount; ++i) {
//            auto data = std::make_unique<ThreadDataForThreads>();
//            data->id = i + 1;
//            data->arr = array.data();
//            data->size = arraySize;
//            data->arrMutex = &arrMutex;
//            threadData.push_back(std::move(data));
//        }
//
//        for (auto& td : threadData) {
//            threads.emplace_back(markerThreads, td.get());
//        }
//
//        while (true) {
//            bool allBlocked = false;
//            while (!allBlocked) {
//                std::this_thread::sleep_for(std::chrono::milliseconds(50));
//                allBlocked = true;
//                for (auto& td : threadData) {
//                    if (td->isActive && !td->blocked)
//                        allBlocked = false;
//                }
//            }
//
//            writeArray(array.data(), arraySize);
//
//            int terminateId;
//            std::cout << "Enter the thread number that will finish the job: ";
//            std::cin >> terminateId;
//
//            if (std::cin.fail() || terminateId <= 0 || terminateId > threadCount || !threadData[terminateId - 1]->isActive) {
//                std::cin.clear();
//                std::cin.ignore(10000, '\n');
//                std::cout << "Invalid id, try again\n\n";
//                continue;
//            }
//
//            auto& td = threadData[terminateId - 1];
//            td->shouldTerminate = true;
//            td->cv.notify_one();
//            td->isActive = false;
//
//            for (auto& el : threadData) {
//                if (el->isActive) {
//                    el->shouldContinue = true;
//                    el->cv.notify_one();
//                }
//            }
//
//            writeArray(array.data(), arraySize);
//
//            bool anyActive = false;
//            for (auto& el : threadData)
//                anyActive |= el->isActive;
//
//            if (!anyActive) break;
//        }
//
//        for (auto& t : threads) if (t.joinable()) t.join();
//
//    }
//    catch (const std::exception& e) {
//        std::cerr << "Error: " << e.what() << "\n";
//        return 1;
//    }
//
//    return 0;
//}