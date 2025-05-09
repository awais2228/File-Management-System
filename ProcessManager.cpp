#include "ProcessManager.h"
#include <iostream>
#include <thread> // Included in original, keeping for completeness though not used for simulation here
#include <chrono> // Included in original, keeping for completeness
#include <algorithm> 
#include <vector> 

ProcessManager::ProcessManager() : nextId(1) {}
int ProcessManager::addProcess(const std::string& taskName) {
    int currentId = nextId++; 
    processQueue.push_back({ currentId, taskName, ProcessStatus::Pending }); 
    std::cout << "Process added: '" << taskName << "' [ID: " << currentId << "]\n";
    return currentId;
}
void ProcessManager::updateProcessStatus(int id, ProcessStatus status) {
    for (auto& proc : processQueue) {
        if (proc.id == id) {
            proc.status = status;
            return;
        }
    }
}
void ProcessManager::showStatus() const {
    if (processQueue.empty()) {
        std::cout << "No processes in the queue.\n";
        return;
    }

    std::cout << "\n--- Process Queue Status ---\n";
    for (const auto& proc : processQueue) {
        std::string statusStr;
        switch (proc.status) {
        case ProcessStatus::Pending: statusStr = "Pending"; break;
        case ProcessStatus::Running: statusStr = "Running"; break;
        case ProcessStatus::Completed: statusStr = "Completed"; break;
        case ProcessStatus::Failed: statusStr = "Failed"; break;
        default: statusStr = "Unknown"; break;
        }
        std::cout << "  [ID: " << proc.id << "] '" << proc.name << "' - " << statusStr << "\n";
    }
    std::cout << "----------------------------\n";
}
void ProcessManager::clearCompleted() {
    clearByStatus(ProcessStatus::Completed);
    std::cout << "Cleared all completed processes.\n";
}
void ProcessManager::clearByStatus(ProcessStatus status) {
    processQueue.erase(
        std::remove_if(processQueue.begin(), processQueue.end(),
            [status](const Process& p) { return p.status == status; }),
        processQueue.end()
    );
}
void ProcessManager::clearAll() {
    processQueue.clear();
    std::cout << "Cleared all processes from the queue.\n";
}
