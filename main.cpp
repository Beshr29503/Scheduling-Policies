#include <iostream>
#include <vector>
#include <queue>
#include <string.h>
#include <cstring>
#include <sstream>
#include <algorithm>
#include <iomanip>

struct Process {
    char name;
    int arrivalTime;
    int serviceTime;
    int finishTime;
    int initialPriority;
    int priority;

    bool operator==(const Process& other) const 
    {
        return name == other.name && arrivalTime == other.arrivalTime && serviceTime == other.serviceTime;
    }
};

class Policy {
protected:
    std::vector<Process> processes;

public:
    virtual void executeTrace(int simulationEnd,bool trace) = 0;
    virtual ~Policy() = default;

    void addProcess(const Process& process) {
        processes.push_back(process);
    };

    void executeStats(std::string policy) {
        std::cout << policy << "\n"; 

        std::cout << "Process    |";
        for (const auto& process : processes) {
            std::cout << "  " << process.name << "  |";
        }
        std::cout << "\n";

        std::cout << "Arrival    |";
        for (const auto& process : processes) {
            std::cout << "  " << process.arrivalTime << "  |";
        }
        std::cout << "\n";

        std::cout << "Service    |";
        for (const auto& process : processes) {
            std::cout << "  " << process.serviceTime << "  |";
        }
        std::cout << " Mean|";
        std::cout << "\n";

        std::cout << "Finish     |";
        for (const auto& process : processes) {
            std::cout << std::setw(3) << std::setfill(' ') << process.finishTime;
            std::cout << std::setw(3) << std::setfill(' ') << "|";        
        }
        std::cout << "-----|";
        std::cout << "\n";

        float mean = 0;
        std::vector<float> turnarounds;

        std::cout << "Turnaround |";
        for (const auto& process : processes) {
            int turnaround = process.finishTime - process.arrivalTime;
            mean += turnaround;
            turnarounds.push_back(turnaround);
            std::cout << std::setw(3) << std::setfill(' ') <<  turnaround;
            std::cout << std::setw(3) << std::setfill(' ') << "|";           
        }
        std::cout << std::setw(5) << std::setfill(' ') << std::fixed << std::setprecision(2) << mean / processes.size() << "|";      
        std::cout << "\n";

        int i = 0;
        mean = 0;

        std::cout << "NormTurn   |";
        for (const auto& process : processes) {
            float normTurn = turnarounds[i] / process.serviceTime;
            mean += normTurn;
            std::cout << std::setw(5) << std::setfill(' ') << std::fixed << std::setprecision(2) << normTurn << "|";
            i++;    
        }
        std::cout << std::setw(5) << std::setfill(' ') << std::fixed << std::setprecision(2) << mean / processes.size() << "|";      
        std::cout << "\n\n";
    }


    void displayTraceTimeline(std::string policy, const std::vector<Process>& processes, const std::vector<std::vector<char>>& timeline, int simulationEnd) {
    std::cout << policy;
    for (int t = 0; t < simulationEnd+1; ++t) {
        std::cout << t % 10 << " ";
    }
    std::cout << "\n" << std::string(simulationEnd * 2 + 8, '-') << "\n";

    for (size_t i = 0; i < processes.size(); ++i) {
        std::cout << processes[i].name << "     ";
        for (int t = 0; t < simulationEnd; ++t) {
            if (timeline[i][t] == '*' || timeline[i][t] == '.') {
                std::cout << "|" << timeline[i][t];
            } else {
                std::cout << "| ";
            }

            if(t == simulationEnd - 1) {
                std::cout << "| ";
            }
        }
        std::cout << "\n";
    }

    std::cout << std::string(simulationEnd * 2 + 8, '-') << "\n";
    std::cout << "\n";
}

};

// ----------------------------------------
class FCFS : public Policy {
public:
    void executeTrace(int simulationEnd, bool trace) {
        std::vector<Process> readyQueue;
        Process currentProcess;
        bool isExecuting = false;
        int currentTime = 0;

        std::vector<std::vector<char>> timeline(processes.size(), std::vector<char>(simulationEnd, ' '));

        while (currentTime < simulationEnd) {
            for (size_t i = 0; i < processes.size(); ++i) {
                if (processes[i].arrivalTime == currentTime) {
                    readyQueue.push_back(processes[i]);
                }
            }

            for (size_t i = 0; i < processes.size(); ++i) {
                if (std::find_if(readyQueue.begin(), readyQueue.end(), [&](const Process& p) { return p.name == processes[i].name; }) != readyQueue.end()) {
                    timeline[i][currentTime] = '.';
                }
            }

            if (!isExecuting && !readyQueue.empty()) {
                currentProcess = readyQueue.front();
                isExecuting = true;
            }

            if (isExecuting) {
                auto it = std::find_if(processes.begin(), processes.end(), [&](const Process& p) { return p.name == currentProcess.name; });
                size_t index = std::distance(processes.begin(), it);
                timeline[index][currentTime] = '*';
                --currentProcess.serviceTime;

                if (currentProcess.serviceTime == 0) {
                    for(auto& process : processes) {
                        if(process.name == currentProcess.name)
                            process.finishTime = currentTime + 1;
                    }
                    currentProcess.finishTime = currentTime + 1;
                    readyQueue.erase(readyQueue.begin());
                    isExecuting = false;
                }
            }
            ++currentTime;
        }

        if(trace)
            displayTraceTimeline("FCFS  ", processes, timeline, simulationEnd);
    }
};

// ----------------------------------------
class RoundRobin : public Policy{
private:
    int quantum;

public:
    void setQuantum(int q) {
        quantum = q;    
    }

void executeTrace(int simulationEnd, bool trace) {
    std::queue<Process> readyQueue;
    int currentTime = 0;
    
    Process temp;
    int pushTime = -1;

    std::vector<std::vector<char>> timeline(processes.size(), std::vector<char>(simulationEnd, ' '));
    std::vector<bool> isInQueue(processes.size(), false);

    while (currentTime < simulationEnd) {
        for (size_t i = 0; i < processes.size(); ++i) {
            if (processes[i].arrivalTime <= currentTime && !isInQueue[i]) {
                readyQueue.push(processes[i]);
                isInQueue[i] = true;
            }
        }

        if(pushTime == currentTime) {
            readyQueue.push(temp);
        }

        if (!readyQueue.empty()) {
            Process currentProcess = readyQueue.front();
            readyQueue.pop();

            int executionTime = std::min(quantum, currentProcess.serviceTime);
            currentProcess.serviceTime -= executionTime;

            for (size_t i = 0; i < processes.size(); ++i) {
            if (processes[i].arrivalTime <= currentTime+executionTime && !isInQueue[i]) {
                readyQueue.push(processes[i]);
                isInQueue[i] = true;
            }
        }

            for (int t = 0; t < executionTime; ++t) {
                auto it = std::find_if(processes.begin(), processes.end(), [&](const Process& p) {
                    return p.name == currentProcess.name;
                });
                size_t index = std::distance(processes.begin(), it);
                if (currentTime + t < simulationEnd) {
                    timeline[index][currentTime + t] = '*';
                }

                std::queue<Process> tempQueue = readyQueue; 
                while (!tempQueue.empty()) {
                    Process queuedProcess = tempQueue.front();
                    tempQueue.pop();

                    auto queuedIt = std::find_if(processes.begin(), processes.end(), [&](const Process& p) {
                        return p.name == queuedProcess.name;
                    });
                    size_t queuedIndex = std::distance(processes.begin(), queuedIt);

                    if (currentTime + t >= processes[queuedIndex].arrivalTime && currentTime + t < simulationEnd) {   
                        timeline[queuedIndex][currentTime + t] = '.';
                    }
                }
            }

            currentTime += executionTime;

            if (currentProcess.serviceTime > 0) {
                temp = currentProcess;
                pushTime = currentTime;
            } else {
                auto it = std::find_if(processes.begin(), processes.end(), [&](const Process& p) {
                    return p.name == currentProcess.name;
                });
                if (it != processes.end()) {
                    it->finishTime = currentTime;
                }
            }
        } else {
            ++currentTime;
        }
    }

    if (trace) {
        displayTraceTimeline("RR-" + std::to_string(quantum) + "  ", processes, timeline, simulationEnd);
    }
}

};

// ----------------------------------------
class SPN : public Policy {
public:
    void executeTrace(int simulationEnd, bool trace) override {
        std::vector<Process> readyQueue;
        Process currentProcess;
        bool isExecuting = false;
        int currentTime = 0;

        std::vector<std::vector<char>> timeline(processes.size(), std::vector<char>(simulationEnd, ' '));

        while (currentTime < simulationEnd) {
            for (size_t i = 0; i < processes.size(); ++i) {
                if (processes[i].arrivalTime == currentTime) {
                    readyQueue.push_back(processes[i]);
                }
            }

            for (size_t i = 0; i < processes.size(); ++i) {
                if (std::find_if(readyQueue.begin(), readyQueue.end(), [&](const Process& p) { return p.name == processes[i].name; }) != readyQueue.end()) {
                    timeline[i][currentTime] = '.';
                }
            }

            if (!isExecuting && !readyQueue.empty()) {
                std::sort(readyQueue.begin(), readyQueue.end(), [](const Process& a, const Process& b) {
                    return a.serviceTime < b.serviceTime;
                });
                currentProcess = readyQueue.front();
                isExecuting = true;
            }

            if (isExecuting) {
                auto it = std::find_if(processes.begin(), processes.end(), [&](const Process& p) { return p.name == currentProcess.name; });
                size_t index = std::distance(processes.begin(), it);
                timeline[index][currentTime] = '*';
                --currentProcess.serviceTime;

                if (currentProcess.serviceTime == 0) {
                    for(auto& process : processes) {
                        if(process.name == currentProcess.name)
                            process.finishTime = currentTime + 1;
                    }
                    currentProcess.finishTime = currentTime + 1;
                    readyQueue.erase(readyQueue.begin());
                    isExecuting = false;
                }
            }
            ++currentTime;
        }

        if(trace)
            displayTraceTimeline("SPN   ", processes, timeline, simulationEnd);
    }

};

// ----------------------------------------
class SRT : public Policy {
public: 
    void executeTrace(int simulationEnd, bool trace) {
        std::vector<Process> readyQueue;
        Process currentProcess;
        bool isExecuting = false;
        int currentTime = 0;

        std::vector<std::vector<char>> timeline(processes.size(), std::vector<char>(simulationEnd, ' '));

        while (currentTime < simulationEnd) {
            for (size_t i = 0; i < processes.size(); ++i) {
                if (processes[i].arrivalTime == currentTime) {
                    readyQueue.push_back(processes[i]);
                }
            }

            for (size_t i = 0; i < processes.size(); ++i) {
                if (std::find_if(readyQueue.begin(), readyQueue.end(), [&](const Process& p) { return p.name == processes[i].name; }) != readyQueue.end()) {
                    timeline[i][currentTime] = '.';
                }
            }

            if (!isExecuting && !readyQueue.empty()) {
                std::sort(readyQueue.begin(), readyQueue.end(), [](const Process& a, const Process& b) {
                    return a.serviceTime < b.serviceTime;
                });
                currentProcess = readyQueue.front();
                isExecuting = true;
            }

            if (isExecuting) {
                auto it = std::find_if(processes.begin(), processes.end(), [&](const Process& p) { return p.name == currentProcess.name; });
                size_t index = std::distance(processes.begin(), it);
                timeline[index][currentTime] = '*';
                --currentProcess.serviceTime;
                readyQueue.front().serviceTime = currentProcess.serviceTime;

                if (currentProcess.serviceTime == 0) {
                    for(auto& process : processes) {
                        if(process.name == currentProcess.name)
                            process.finishTime = currentTime + 1;
                    }
                    currentProcess.finishTime = currentTime + 1;
                    readyQueue.erase(readyQueue.begin());
                }
                isExecuting = false;
            }
            ++currentTime;
        }

        if(trace)
            displayTraceTimeline("SRT   ", processes, timeline, simulationEnd);
    }
};

// ----------------------------------------
class HRRN : public Policy {
public:
    void executeTrace(int simulationEnd, bool trace) override {
    std::vector<Process> readyQueue;
    Process currentProcess;
    bool isExecuting = false;
    int currentTime = 0;

    std::vector<std::vector<char>> timeline(processes.size(), std::vector<char>(simulationEnd, ' '));

    while (currentTime < simulationEnd) {
        for (size_t i = 0; i < processes.size(); ++i) {
            if (processes[i].arrivalTime == currentTime) {
                readyQueue.push_back(processes[i]);
            }
        }

        for (size_t i = 0; i < processes.size(); ++i) {
            if (std::find_if(readyQueue.begin(), readyQueue.end(), [&](const Process& p) { return p.name == processes[i].name; }) != readyQueue.end()) {
                timeline[i][currentTime] = '.';
            }
        }

        if (!isExecuting && !readyQueue.empty()) {
            float maxRR = -1.0; 
            size_t selectedIndex = 0;

            for (size_t i = 0; i < readyQueue.size(); ++i) {
                int waitingTime = currentTime - readyQueue[i].arrivalTime;
                float responseRatio = (waitingTime + readyQueue[i].serviceTime) / float(readyQueue[i].serviceTime);

                if (responseRatio > maxRR) {
                    maxRR = responseRatio;
                    selectedIndex = i;
                }
            }

            currentProcess = readyQueue[selectedIndex];
            readyQueue.erase(readyQueue.begin() + selectedIndex);
            isExecuting = true;
        }

        if (isExecuting) {
            auto it = std::find_if(processes.begin(), processes.end(), [&](const Process& p) { return p.name == currentProcess.name; });
            size_t index = std::distance(processes.begin(), it);
            timeline[index][currentTime] = '*';
            --currentProcess.serviceTime;

            if (currentProcess.serviceTime == 0) {
                for (auto& process : processes) {
                    if (process.name == currentProcess.name)
                        process.finishTime = currentTime + 1;
                }
                isExecuting = false;
            }
        }
        ++currentTime;
    }

    if (trace) {
        displayTraceTimeline("HRRN  ", processes, timeline, simulationEnd);
    }
}
};

// ----------------------------------------
class Feedback1 : public Policy {
public:
    void executeTrace(int simulationEnd, bool trace) {
        std::vector<std::queue<Process>> readyQueues;
        int currentTime = 0;
        int processNum = 0;

        std::vector<std::vector<char>> timeline(processes.size(), std::vector<char>(simulationEnd, ' '));

        std::vector<bool> isInQueue(processes.size(), false);

        Process currentProcess;
        int remainingQuantum = 0;
        bool isExecuting = false;

        while (currentTime < simulationEnd) {
            for (size_t i = 0; i < processes.size(); ++i) {
                if (processes[i].arrivalTime == currentTime && !isInQueue[i]) {
                    if (readyQueues.empty()) {
                        readyQueues.emplace_back(); 
                    }
                    readyQueues[0].push(processes[i]);
                    isInQueue[i] = true;
                    processNum++;
                }
            }

            for (size_t i = 0; i < processes.size(); ++i) {
                bool inQueue = false;
                for (const auto& queue : readyQueues) {
                    std::queue<Process> tempQueue = queue;
                    while (!tempQueue.empty()) {
                        if (tempQueue.front().name == processes[i].name) {
                            inQueue = true;
                            break;
                        }
                        tempQueue.pop();
                    }
                    if (inQueue) break;
                }
                if (inQueue && !(isExecuting && currentProcess.name == processes[i].name)) {
                    timeline[i][currentTime] = '.';
                }
            }

            if (!isExecuting) {
                for (size_t i = 0; i < readyQueues.size(); ++i) {
                    if (!readyQueues[i].empty()) {
                        currentProcess = readyQueues[i].front();
                        remainingQuantum = 1; 
                        isExecuting = true;
                        break;
                    }
                    else if(processNum == 1) {
                        if(i == 0) {
                            readyQueues[i].push(readyQueues[i+1].front());
                            readyQueues[i+1].pop();
                            currentProcess = readyQueues[i].front();
                            remainingQuantum = 1 << i;
                            isExecuting = true;
                            break;
                        }
                    }
                }
            }

            if (isExecuting) {
                auto it = std::find_if(processes.begin(), processes.end(), [&](const Process& p) {
                    return p.name == currentProcess.name;
                });
                size_t index = std::distance(processes.begin(), it);
                timeline[index][currentTime] = '*';

                --currentProcess.serviceTime;
                --remainingQuantum;
                
                if (currentProcess.serviceTime == 0 || remainingQuantum == 0) {
                    for (size_t i = 0; i < readyQueues.size(); ++i) {
                        if (!readyQueues[i].empty() && readyQueues[i].front().name == currentProcess.name) {
                            readyQueues[i].pop();
                            if (remainingQuantum == 0 && currentProcess.serviceTime > 0) {
                                size_t nextQueueIndex = i + 1;
                                if (nextQueueIndex >= readyQueues.size()) {
                                    readyQueues.emplace_back(); 
                                }
                                readyQueues[nextQueueIndex].push(currentProcess); 
                            } else if (currentProcess.serviceTime == 0) {
                                auto it = std::find_if(processes.begin(), processes.end(), [&](const Process& p) {
                                    return p.name == currentProcess.name;
                                });
                                if (it != processes.end()) {
                                    it->finishTime = currentTime + 1;
                                }
                            }
                            break;
                        }
                    }
                    isExecuting = false;
                }
            }
            ++currentTime;
        }

        if (trace) {
            displayTraceTimeline("FB-1  ", processes, timeline, simulationEnd);
        }
    }
};

// ----------------------------------------
class Feedback2i : public Policy {
public:
    void executeTrace(int simulationEnd, bool trace) override {
        std::vector<std::queue<Process>> readyQueues;
        int currentTime = 0;
        int processNum = 0;

        std::vector<std::vector<char>> timeline(processes.size(), std::vector<char>(simulationEnd, ' '));

        std::vector<bool> isInQueue(processes.size(), false);

        Process currentProcess;
        int remainingQuantum = 0;
        bool isExecuting = false;

        while (currentTime < simulationEnd) {
            for (size_t i = 0; i < processes.size(); ++i) {
                if (processes[i].arrivalTime == currentTime && !isInQueue[i]) {
                    if (readyQueues.empty()) {
                        readyQueues.emplace_back(); 
                    }
                    readyQueues[0].push(processes[i]);
                    isInQueue[i] = true;
                    processNum++;
                }
            }

            for (size_t i = 0; i < processes.size(); ++i) {
                bool inQueue = false;
                for (const auto& queue : readyQueues) {
                    std::queue<Process> tempQueue = queue;
                    while (!tempQueue.empty()) {
                        if (tempQueue.front().name == processes[i].name) {
                            inQueue = true;
                            break;
                        }
                        tempQueue.pop();
                    }
                    if (inQueue) break;
                }
                if (inQueue && !(isExecuting && currentProcess.name == processes[i].name)) {
                    timeline[i][currentTime] = '.';
                }
            }

            if (!isExecuting) {
                for (size_t i = 0; i < readyQueues.size(); ++i) {
                    if (!readyQueues[i].empty()) {
                        currentProcess = readyQueues[i].front();
                        remainingQuantum = 1 << i; 
                        isExecuting = true;
                        break;
                    }
                    else if(processNum == 1) {
                        if(i == 0) {
                            readyQueues[i].push(readyQueues[i+1].front());
                            readyQueues[i+1].pop();
                            currentProcess = readyQueues[i].front();
                            remainingQuantum = 1 << i;
                            isExecuting = true;
                            break;
                        }
                    }
                }
            }

            if (isExecuting) {
                auto it = std::find_if(processes.begin(), processes.end(), [&](const Process& p) {
                    return p.name == currentProcess.name;
                });
                size_t index = std::distance(processes.begin(), it);
                timeline[index][currentTime] = '*';

                --currentProcess.serviceTime;
                --remainingQuantum;
                
                if (currentProcess.serviceTime == 0 || remainingQuantum == 0) {
                    for (size_t i = 0; i < readyQueues.size(); ++i) {
                        if (!readyQueues[i].empty() && readyQueues[i].front().name == currentProcess.name) {
                            readyQueues[i].pop();
                            if (remainingQuantum == 0 && currentProcess.serviceTime > 0) {
                                size_t nextQueueIndex = i + 1;
                                if (nextQueueIndex >= readyQueues.size()) {
                                    readyQueues.emplace_back(); 
                                }
                                readyQueues[nextQueueIndex].push(currentProcess); 
                            } else if (currentProcess.serviceTime == 0) {
                                auto it = std::find_if(processes.begin(), processes.end(), [&](const Process& p) {
                                    return p.name == currentProcess.name;
                                });
                                if (it != processes.end()) {
                                    it->finishTime = currentTime + 1;
                                }
                            }
                            break;
                        }
                    }
                    isExecuting = false;
                }
            }
            ++currentTime;
        }

        if (trace) {
            displayTraceTimeline("FB-2i ", processes, timeline, simulationEnd);
        }
    }
};

// ----------------------------------------
class Aging : public Policy {
private:
    int quantum;

public:
    void setQuantum(int q) {
        quantum = q;    
    }

    void executeTrace(int simulationEnd, bool trace) {
        std::queue<Process> readyQueue;
        std::vector<Process> runningQueue;
        int currentTime = 0;

        std::vector<std::vector<char>> timeline(processes.size(), std::vector<char>(simulationEnd, ' '));
        std::vector<bool> isInQueue(processes.size(), false);

        while (currentTime < simulationEnd) {
        for (size_t i = 0; i < processes.size(); ++i) {
            if (processes[i].arrivalTime <= currentTime && !isInQueue[i]) {
                readyQueue.push(processes[i]);
                isInQueue[i] = true;
            }
        }


        if (!readyQueue.empty()) {
            Process currentProcess = readyQueue.front();
            runningQueue.push_back(currentProcess);

            for (size_t i = 0; i < processes.size(); ++i) {
            if (processes[i].arrivalTime <=  currentTime+quantum && !isInQueue[i]) {
                readyQueue.push(processes[i]);
                isInQueue[i] = true;
            }
        }

            for (int t = 0; t < quantum; ++t) {
                std::queue<Process> tempQueue = readyQueue; 
                while (!tempQueue.empty()) {
                    Process queuedProcess = tempQueue.front();
                    tempQueue.pop();

                    auto queuedIt = std::find_if(processes.begin(), processes.end(), [&](const Process& p) {
                        return p.name == queuedProcess.name;
                    });
                    size_t queuedIndex = std::distance(processes.begin(), queuedIt);

                    if (currentTime + t >= processes[queuedIndex].arrivalTime && currentTime + t < simulationEnd) {   
                        timeline[queuedIndex][currentTime + t] = '.';
                    }
                }

                auto it = std::find_if(processes.begin(), processes.end(), [&](const Process& p) {
                    return p.name == currentProcess.name;
                });
                size_t index = std::distance(processes.begin(), it);
                if (currentTime + t < simulationEnd) {
                    timeline[index][currentTime + t] = '*';
                }
            }

            currentTime += quantum;
            std::queue<Process> p;
            
            while(!readyQueue.empty()) {
                if(readyQueue.front() == currentProcess) {
                    readyQueue.front().priority = currentProcess.initialPriority;
                    p.push(readyQueue.front());
                    readyQueue.pop();
                }
                else  {
                    readyQueue.front().priority++;
                    p.push(readyQueue.front());
                    readyQueue.pop();
                } 
            }

            while(!p.empty()) {
                readyQueue.push(p.front());
                p.pop();
            }
           
            std::vector<Process> temp;
            while (!readyQueue.empty()) {
                temp.push_back(readyQueue.front());
                readyQueue.pop();
            }

            std::sort(temp.begin(), temp.end(), [](const Process& a, const Process& b) {
                return a.priority > b.priority; 
            });

            int highestPriority = temp[0].priority;
            int i = 0;
            std::vector<Process> conflictProcesses;
            std::vector<Process> solvedProcesses;

            while(temp.size() > 1 && temp[i].priority == highestPriority && temp[i].priority == temp[i+1].priority) {
                conflictProcesses.push_back(temp[i]);
                i++;
            }
            conflictProcesses.push_back(temp[i]);
            
            if(conflictProcesses.size() == 2) {
                if(temp[0].priority == temp[1].priority) {
                    for (int i = runningQueue.size() - 1; i >= 0; --i) {
                        if(temp[0] == runningQueue[i]) {
                            temp.push_back(temp[0]);
                            temp.erase(temp.begin());
                            break;
                        } else if (temp[1] == runningQueue[i]) {
                            break;
                        }
                    }
                
                }
            } else {
                int j;
                if(temp[0].priority == temp[1].priority) {
                    for (int i = runningQueue.size() - 1; i >= 0; --i) {
                        for(auto& p1 : conflictProcesses) {
                            if(p1 == runningQueue[i]) {
                                j = 0;
                                for(auto& p2 : solvedProcesses) {
                                    if(p2.name == p1.name) {
                                        j++;
                                        break;
                                    }
                                }
                                if(j == 0)
                                    solvedProcesses.push_back(p1);
                            }
                        }
                    }

                    j = 0;
                    for(int i = solvedProcesses.size() - 1; i >= 0; --i) {
                        temp[j] = solvedProcesses[i];
                        j++;
                    }
                }
            }
         
            for (auto& process : temp) {
                readyQueue.push(process);
            }

        } else {
            ++currentTime;
        }
    }

    if (trace) {
        displayTraceTimeline("Aging " , processes, timeline, simulationEnd);
    }
    }
};


// ----------------------------------------
void executePolicy1(std::vector<Process> processes, std::string mode, std::string policiesInput, int simulationEnd) {
    if (policiesInput.find("1") != std::string::npos) {
            FCFS fcfs;
            for (const auto& process : processes) {
                fcfs.addProcess(process);
            }

            if (mode == "trace") {
                fcfs.executeTrace(simulationEnd,true);
            } else if (mode == "stats") {
                fcfs.executeTrace(simulationEnd,false);
                fcfs.executeStats("FCFS");
            } else {
                std::cerr << "Error: Unknown mode " << mode << ".\n";
            }
        } else if (policiesInput.find("3") != std::string::npos) {
            SPN spn;
            for (const auto& process : processes) {
                spn.addProcess(process);
            }

            if (mode == "trace") {
                spn.executeTrace(simulationEnd,true);
            } else if (mode == "stats") {
                spn.executeTrace(simulationEnd,false);
                spn.executeStats("SPN");
            } else {
                std::cerr << "Error: Unknown mode " << mode << ".\n";
            }
        } else if (policiesInput.find("4") != std::string::npos) {
            SRT srt;
            for (const auto& process : processes) {
                srt.addProcess(process);
            }

            if (mode == "trace") {
                srt.executeTrace(simulationEnd,true);
            } else if (mode == "stats") {
                srt.executeTrace(simulationEnd,false);
                srt.executeStats("SRT");
            } else {
                std::cerr << "Error: Unknown mode " << mode << ".\n";
            }
        } else if (policiesInput.find("5") != std::string::npos) { 
            HRRN hrrn;
            for (const auto& process : processes) {
                hrrn.addProcess(process);
            }

            if (mode == "trace") {
                hrrn.executeTrace(simulationEnd, true);
            } else if (mode == "stats") {
                hrrn.executeTrace(simulationEnd, false);
                hrrn.executeStats("HRRN");
            } else {
                std::cerr << "Error: Unknown mode " << mode << ".\n";
            }
        } else if (policiesInput.find("6") != std::string::npos) { 
            Feedback1 f1;
            for (const auto& process : processes) {
                f1.addProcess(process);
            }

            if (mode == "trace") {
                f1.executeTrace(simulationEnd, true);
            } else if (mode == "stats") {
                f1.executeTrace(simulationEnd, false);
                f1.executeStats("FB-1");
            } else {
                std::cerr << "Error: Unknown mode " << mode << ".\n";
            }
        } else if (policiesInput.find("7") != std::string::npos) { 
            Feedback2i f2i;
            for (const auto& process : processes) {
                f2i.addProcess(process);
            }

            if (mode == "trace") {
                f2i.executeTrace(simulationEnd, true);
            } else if (mode == "stats") {
                f2i.executeTrace(simulationEnd, false);
                f2i.executeStats("FB-2i");
            } else {
                std::cerr << "Error: Unknown mode " << mode << ".\n";
            }
        }
}

void executePolicy2(std::vector<Process> processes, std::string mode, std::string policiesInput, int simulationEnd) {
    char temp[policiesInput.length() + 1];
        strcpy(temp, policiesInput.c_str());

        char* policy = strtok(temp, "-"); 
        char* quantum = strtok(NULL, "-"); 

        if (policy && quantum) {
            int q = std::stoi(quantum); 

            if (std::string(policy) == "2") {
                RoundRobin rr;

                for (const auto& process : processes) {
                rr.addProcess(process);
                }

                rr.setQuantum(q);

                if (mode == "trace") {
                    rr.executeTrace(simulationEnd, true);
                } else if (mode == "stats") {
                    rr.executeTrace(simulationEnd, false);
                    rr.executeStats("RR-" + std::string(quantum));
                } else {
                    std::cerr << "Error: Unknown mode " << mode << ".\n";
                }
            } else if (std::string(policy) == "8") {
                Aging aging;

                for (const auto& process : processes) {
                aging.addProcess(process);
                }

                aging.setQuantum(q);

                if (mode == "trace") {
                    aging.executeTrace(simulationEnd, true);
                } else {
                    std::cerr << "Error: Unknown mode " << mode << ".\n";
                }
            }
        }
}

int main() {
    std::string mode;
    if (!(std::cin >> mode)) {
        std::cerr << "Error: Failed to read mode from input.\n";
        return 1;
    }

    std::string policiesInput;
    std::cin.ignore();
    if (!std::getline(std::cin, policiesInput)) {
        std::cerr << "Error: Failed to read policies from input.\n";
        return 1;
    }

    int simulationEnd, numProcesses;
    if (!(std::cin >> simulationEnd >> numProcesses)) {
        std::cerr << "Error: Failed to read simulation end time or number of processes.\n";
        return 1;
    }

    std::cin.ignore();

    std::vector<Process> processes;
    for (int i = 0; i < numProcesses; ++i) {
        std::string processLine;
        if (!std::getline(std::cin, processLine)) {
            std::cerr << "Error: Failed to read process line " << i + 1 << ".\n";
            return 1;
        }

        std::istringstream ss(processLine);
        Process p;
        char comma;
        if(!(policiesInput.length() > 1 && policiesInput[0] == '8'))
        {
            if (!(ss >> p.name >> comma >> p.arrivalTime >> comma >> p.serviceTime)) {
                std::cerr << "Error: Failed to parse process line " << i + 1 << ".\n";
                return 1;
            }
            processes.push_back(p);
        } else {
            if (!(ss >> p.name >> comma >> p.arrivalTime >> comma >> p.initialPriority)) {
                std::cerr << "Error: Failed to parse process line " << i + 1 << ".\n";
                return 1;
            }
            p.priority = p.initialPriority;
            processes.push_back(p);   
        }
    }

    if(policiesInput.length() == 1) {
        executePolicy1(processes, mode, policiesInput, simulationEnd);
    } else if(policiesInput.length() == 3) {
        executePolicy2(processes, mode, policiesInput, simulationEnd);
    } else if(policiesInput.length() > 3) {
        std::stringstream ss(policiesInput);
        std::string policy;

        while (std::getline(ss, policy, ',')) {
            if(policy.length() == 1) {
                executePolicy1(processes, mode, policy, simulationEnd);
            } else {
                executePolicy2(processes, mode, policy, simulationEnd);
            }
        }
    } else {
    std::cerr << "Error: Unknown Policy.\n";
    return 1;
    }
    return 0;
}
