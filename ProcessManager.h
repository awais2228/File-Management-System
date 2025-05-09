#pragma once
#include <string>
#include <vector>
#include <algorithm> 

enum class ProcessStatus {
    Pending,   
    Running,   
    Completed, 
    Failed     
};

struct Process {
    int id;           
    std::string name; 
    ProcessStatus status; 
};
class ProcessManager {
private:
    std::vector<Process> processQueue; 
    int nextId;

public:
    ProcessManager();
    int addProcess(const std::string& taskName);
    void updateProcessStatus(int id, ProcessStatus status);
    void showStatus() const;
    void clearCompleted();
    void clearByStatus(ProcessStatus status);
    void clearAll();
};
