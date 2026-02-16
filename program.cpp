#include <string>
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
    }
    
    return 0;
}