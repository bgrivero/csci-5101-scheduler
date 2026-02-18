#include <string>
#include <vector>
#include <iostream>
using namespace std;

/*
Process struct. Holds each process arrival time, burst time, remaining time, nice level, 
index for tie-breaks, and performance metrics.
*/
struct Process{
    int id;
    int arrival;
    int burst;
    int remaining;
    int nice;
    int start_time = -1;
    int completion_time = 0;
    int waiting_time = 0;
    int turnaround_time = 0;
    int response_time = 0;
    
    Process(int id, int arrival, int burst, int nice){
        this->id = id;
        this->arrival = arrival;
        this->burst = burst;
        this->remaining = burst;
        this->nice = nice;
    }
};

/*
Test Case struct used to keep track of each test cases.
*/
struct TestCase {
    string algorithm;
    int quantum;
    Process** processes; 
    int size;
    
    TestCase(int size, string algorithm){
        this->quantum = 0;
        this->algorithm = algorithm;
        this->size = size;
        processes = new Process*[size];
    }
    
    void addProcess(int idx, Process* proc){
        processes[idx] = proc;
    }
    
    void printTestCase(int testNumber){
        cout << "Test Case " << testNumber << ": " << algorithm;
        if (algorithm == "RR"){
            cout << " (Q=" << quantum << ")";
        }
        cout << "\n";
        for (int i = 0; i < size; i++){
            Process* p = processes[i];
            cout << "  Process " << p->id
                << " | Arrival: " << p->arrival
                << " | Burst: " << p->burst
                << " | Nice: " << p->nice
                << "\n";
        }
        cout << "--------------------------\n";
    }
    
    ~TestCase(){
        for (int i = 0; i < size; i++){
            delete processes[i];
        }
        delete[] processes;
    }
};

void printResults(int testNumber, TestCase* tc) {
    int n = tc->size;
    Process** procs = tc->processes;

    // compute metrics for each process
    int totalBurst = 0;
    int totalTime = 0;
    for (int i = 0; i < n; i++) {
        Process* p = procs[i];
        p->turnaround_time = p->completion_time - p->arrival;
        p->waiting_time    = p->turnaround_time - p->burst;
        p->response_time   = p->start_time - p->arrival;
        totalBurst += p->burst;
        if (p->completion_time > totalTime) totalTime = p->completion_time;
    }

    int cpuUtil = (int)((double)totalBurst / totalTime * 100);
    double throughput = (double)n / totalTime;

    cout << "Total time elapsed: " << totalTime << "ns" << endl;
    cout << "Total CPU burst time: " << totalBurst << "ns" << endl;
    cout << "CPU Utilization: " << cpuUtil << "%" << endl;
    cout << "Throughput: " << throughput << " processes/ns" << endl;

    // waiting times
    double avgWait = 0;
    cout << "Waiting times:" << endl;
    for (int i = 0; i < n; i++) {
        cout << " Process " << procs[i]->id << ": " << procs[i]->waiting_time << "ns" << endl;
        avgWait += procs[i]->waiting_time;
    }
    avgWait /= n;
    cout << "Average waiting time: " << avgWait << "ns" << endl;

    // turnaround times
    double avgTurn = 0;
    cout << "Turnaround times:\n";
    for (int i = 0; i < n; i++) {
        cout << " Process " << procs[i]->id << ": " << procs[i]->turnaround_time << "ns" << endl;
        avgTurn += procs[i]->turnaround_time;
    }
    avgTurn /= n;
    cout << "Average turnaround time: " << avgTurn << "ns" << endl;

    // response times
    double avgResp = 0;
    cout << "Response times:\n";
    for (int i = 0; i < n; i++) {
        cout << " Process " << procs[i]->id << ": " << procs[i]->response_time << "ns" << endl;
        avgResp += procs[i]->response_time;
    }
    avgResp /= n;
    cout << "Average response time: " << avgResp << "ns" << endl;
}

void runFCFS(int testNumber, TestCase* tc){
    // Print the test number and algorithm name to match output for gantt "chart".
    cout << testNumber << " " << tc->algorithm << endl;
    int n = tc->size;
    Process** processes = tc->processes;
    
    vector<bool> done(n, false);
    int completed = 0;
    int currentTime= 0;
    
    while (completed < n){
        int chosenID = -1;
        for (int i = 0; i < n; i++){
            if (done[i]) continue;
            if (processes[i]->arrival > currentTime) continue;

            if (chosenID == -1){
                chosenID = i;
            }
            else {
                Process* best = processes[chosenID];
                Process* cur = processes[i];
                if (cur->arrival < best->arrival || cur->arrival == best->arrival && cur->id < best->id){
                    chosenID = i;
                }
            }
        }
        
        // Handle CPU being idle by skipping to next time where a process arrives
        if (chosenID == -1){
            int timeJump = 5000; // Temporary value that is high 
            for (int i = 0; i < n; i++){
                if (!done[i] && processes[i]->arrival < timeJump){
                    timeJump = processes[i]->arrival;
                }
            }
            currentTime = timeJump;
            continue;
        }

        Process* p = processes[chosenID];
        if (p->start_time == -1){
            p->start_time = currentTime;
        }

        cout << currentTime << " " << chosenID << " " << p->burst << "X" << endl;
        currentTime += p->burst;
        p->remaining = 0;
        p->completion_time = currentTime;
        done[chosenID] = true;
        completed ++;
    }
    printResults(testNumber, tc);
};

void runSRTF(int testNumber, TestCase* tc){
    int n = tc->size;
    Process** processes = tc->processes;
    
    vector<bool> done(n, false);
    int completed = 0;
    int currentTime= 0;

    while (completed < n){
        int chosenID = -1;
        for (int i = 0; i < n; i++){
            if (done[i]) continue;
            if (processes[i]->arrival > currentTime) continue;

            if (chosenID == -1){
                chosenID = i;
            }
            else {
                Process* best = processes[chosenID];
                Process* cur = processes[i];
                if (cur->remaining < best->remaining){
                    chosenID = i;
                }
                else if (cur->remaining == best->remaining) {
                    if (cur->arrival < best->arrival || cur->arrival == best->arrival && cur->id < best->id){
                        chosenID = i;
                    }
                }
            }
        }
        
        // Handle CPU being idle by skipping to next time where a process arrives
        if (chosenID == -1){
            int timeJump = 5000; // Temporary value that is high 
            for (int i = 0; i < n; i++){
                if (!done[i] && processes[i]->arrival < timeJump){
                    timeJump = processes[i]->arrival;
                }
            }
            currentTime = timeJump;
            continue;
        }

        Process* p = processes[chosenID];
        if (p->start_time == -1){
            p->start_time = currentTime;
        }

        int runUntil = currentTime + p->remaining;

        for (int i = 0; i < n; i++){
            if (done[i] || i == chosenID){
                continue;
            }
            int arrivalTime = processes[i]->arrival;
            if (arrivalTime > currentTime && arrivalTime < runUntil){
                int remainingAtArrival = p->remaining - (arrivalTime - currentTime);
                int newRem = processes[i]->remaining;
                bool preempt = false;
                if (newRem < remainingAtArrival){
                    preempt = true;
                }
                else if (newRem == remainingAtArrival){
                    if (processes[i]->arrival < p->arrival || processes[i]->arrival == p->arrival && processes[i]->id < p->id){
                        preempt = true;
                    }
                }
                if (preempt){
                    runUntil = arrivalTime;
                }
            }
        }

        int runDuration = runUntil - currentTime;
        p->remaining -= runDuration;

        bool finished = (p->remaining == 0);
        if (finished){
            p->completion_time = currentTime + runDuration;
            done[chosenID] = true;
            completed ++;
        }
        currentTime += runDuration;
    }
    printResults(testNumber, tc);
}

int main(void){
    int num_test;
    cin >> num_test;
    
    // Iterates through each test case
    for (int i = 0; i < num_test; i++){
        int num_process;
        string algorithm;
        
        cin >> num_process >> algorithm;
        TestCase* testCase = new TestCase(num_process, algorithm);
        
        // Iterates through each process per test case
        for (int j = 0; j < num_process; j++){
            int arrival, burst, nice;
            cin >> arrival >> burst >> nice;
            Process* proc = new Process(j, arrival, burst, nice);
            testCase->addProcess(j, proc);
        }
        // Temporary print function to keep track of processes
        testCase->printTestCase(i + 1);
        if (algorithm == "FCFS"){
            runFCFS(i, testCase);
        }
        else if (algorithm == "SRTF"){
            runSRTF(i, testCase);
        }
    }
    
    return 0;
}