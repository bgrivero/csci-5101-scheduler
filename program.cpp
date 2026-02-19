#include <string>
#include <vector>
#include <limits.h>
#include <iostream>
#include <algorithm>
#include <queue>
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
    
    // Serves as something like an array to keep track of completed processes.
    // Tracked by index (n) and becomes true once completed, this is updated at the end of the function.
    vector<bool> done(n, false);
    int completed = 0;
    int currentTime= 0;
    
    // Main meat of the algorithm, it runs until all completed = n, meaning all processes have finished.
    while (completed < n){
        // Initialized as -1 for placeholder, this means that there is no process that has been chosen yet.
        int chosenID = -1;
        // Iterates over all processes.
        for (int i = 0; i < n; i++){
            if (done[i]) continue; // This just skips the process if it is already skipped.
            if (processes[i]->arrival > currentTime) continue; // This just skips processes that have not yet arrived.

            if (chosenID == -1){
                chosenID = i;
            }
            // In the case that there is a chosen process, it is compared with the ith process.
            // Comparisons are: which process arrived first, and if they arrived at the same time, then who has a lower id.
            else {
                Process* chosen = processes[chosenID];
                Process* ith = processes[i];
                if (ith->arrival < chosen->arrival || ith->arrival == chosen->arrival && ith->id < chosen->id){
                    chosenID = i;
                }
            }
        }
        
        // Handles CPU being idle by skipping to the next time where a process arrives.
        if (chosenID == -1){
            int timeJump = INT_MAX;
            // Iterates over all the processes and sets timeJump to the earliest arrival of a new process.
            for (int i = 0; i < n; i++){
                if (!done[i] && processes[i]->arrival < timeJump){
                    timeJump = processes[i]->arrival;
                }
            }
            currentTime = timeJump;
            continue;
        }

        // Process has finally been chosen by now, and so it starts to run now.
        Process* p = processes[chosenID];
        if (p->start_time == -1){
            p->start_time = currentTime;
        }

        // Output for gantt "chart".
        cout << currentTime << " " << p->id << " " << p->burst << "X" << endl;
        currentTime += p->burst;
        p->remaining = 0;
        p->completion_time = currentTime;
        done[chosenID] = true;
        completed ++;
    }
    printResults(testNumber, tc);
};

void runSRTF(int testNumber, TestCase* tc){
    // Print the test number and algorithm name to match output for gantt "chart".
    cout << testNumber << " " << tc->algorithm << endl;
    int n = tc->size;
    Process** processes = tc->processes;
    
    // Serves as something like an array to keep track of completed processes.
    // Tracked by index (n) and becomes true once completed, this is updated at the end of the function.
    vector<bool> done(n, false);
    int completed = 0;
    int currentTime= 0;

    // Main meat of the algorithm, it runs until all completed = n, meaning all processes have finished.
    while (completed < n){
        // Initialized as -1 for placeholder, this means that there is no process that has been chosen yet.
        int chosenID = -1;
        // Iterates over all processes.
        for (int i = 0; i < n; i++){
            if (done[i]) continue; // This just skips the process if it is already skipped.
            if (processes[i]->arrival > currentTime) continue; // This just skips processes that have not yet arrived.

            if (chosenID == -1){
                chosenID = i;
            }
            // In the case that there is a chosen process, it is compared with the ith process.
            // Comparisons are: which process has shorter remaining time, and in the case of a tie, apply the fcfs logic.
            else {
                Process* chosen = processes[chosenID];
                Process* ith = processes[i];
                if (ith->remaining < chosen->remaining){
                    chosenID = i;
                }
                else if (ith->remaining == chosen->remaining) {
                    if (ith->arrival < chosen->arrival || ith->arrival == chosen->arrival && ith->id < chosen->id){
                        chosenID = i;
                    }
                }
            }
        }
        
        // Handle CPU being idle by skipping to next time where a process arrives
        if (chosenID == -1){
            int timeJump = INT_MAX;
            // Iterates over all the processes and sets timeJump to the earliest arrival of a new process.
            for (int i = 0; i < n; i++){
                if (!done[i] && processes[i]->arrival < timeJump){
                    timeJump = processes[i]->arrival;
                }
            }
            currentTime = timeJump;
            continue;
        }

        // Process has finally been chosen by now, and so it starts to run now.
        Process* p = processes[chosenID];
        if (p->start_time == -1){
            p->start_time = currentTime;
        }

        // Duration the currently running process has until it terminates or is preempted.
        int runUntil = currentTime + p->remaining;

        for (int i = 0; i < n; i++){
            if (done[i] || i == chosenID){
                continue;
            }
            int incomingArrivalTime = processes[i]->arrival; // Save arrival time of currently selected process.
            // Preemption happens here, conditional checker that ensures incoming process is actually new and will arrive while chosen process is running.
            if (incomingArrivalTime > currentTime && incomingArrivalTime < runUntil){
                int remainingAtArrival = p->remaining - (incomingArrivalTime - currentTime);
                int incomingRemaining = processes[i]->remaining;
                bool preempt = false;
                if (incomingRemaining < remainingAtArrival){
                    preempt = true;
                }
                else if (incomingRemaining == remainingAtArrival){
                    if (processes[i]->arrival < p->arrival || processes[i]->arrival == p->arrival && processes[i]->id < p->id){
                        preempt = true;
                    }
                }
                if (preempt){
                    runUntil = incomingArrivalTime;
                }
            }
        }

        // Update the remaining time of the process currently running.
        int runDuration = runUntil - currentTime;
        p->remaining -= runDuration;

        // Handles completed processes.
        bool finished = (p->remaining == 0);
        if (finished){
            p->completion_time = currentTime + runDuration;
            done[chosenID] = true;
            completed ++;
            cout << currentTime << " " << p->id << " " << runDuration << "X" << endl;
        }
        else {
            cout << currentTime << " " << p->id << " " << runDuration << endl;
        }
        currentTime += runDuration;
    }
    printResults(testNumber, tc);
}

struct _SJFcomp{
    bool operator()(Process* a, Process*b){
         if (a->burst != b->burst)
            return a->burst > b->burst;
        if (a->arrival != b->arrival)
            return a->arrival > b->arrival;
        return a->id > b->id;
    }
};

void runSJF(int testNumber, TestCase* tc){
    cout << testNumber << " " << tc->algorithm << endl;
    int n = tc->size;
    Process** processes = tc->processes;
    int completed=0;
    int currentTime= 0;
    int idx = 0;

    // sort processes by arrival time, ascending
    sort(processes, processes +n, [](Process* a, Process*b){
        if (a->arrival != b->arrival){
            return a->arrival < b-> arrival;
        } else{
            return a->id < b->id;
        }
    });

    // creates a priority queue to store the processes by burst time to simplify code
    priority_queue<Process*, vector<Process*>, _SJFcomp> ready_queue;

    while(completed<n){
        // keep adding to the ready_queue all processes that have arrived.
        while(idx<n && processes[idx]->arrival <= currentTime){
            ready_queue.push(processes[idx]);
            idx++;
        }

        // once you have emptied the ready queue, jump to the next process
        if (ready_queue.empty()){
            currentTime = processes[idx]->arrival;
            continue;
        }
        // from the ready queue, get the shortest job
        Process* p = ready_queue.top();
        ready_queue.pop();

        p->start_time = currentTime;
        p->completion_time = currentTime + p->burst;
        p->turnaround_time = p->completion_time - p->arrival; 
        p->waiting_time = p->turnaround_time - p->burst;
        p->response_time = currentTime - p->arrival;
        currentTime += p->burst;
        completed ++;

        cout << currentTime << " " << p->id << " " << p-> burst <<'X'<< endl;
    }

    // sort it back by id order
    sort(processes, processes + n, [](Process* a, Process* b){
    return a->id < b->id;
    });
    printResults(testNumber,tc);
    
}

struct _PrioComp{
    bool operator()(Process* a, Process*b){
        if (a->nice != b->nice)
            return a->nice > b->nice;
        if (a->arrival != b->arrival)
            return a->arrival > b->arrival;
        return a->id > b->id;
    }
};

void runPrio(int testNumber, TestCase* tc){
    cout << testNumber << " " << tc->algorithm << endl;
    int n = tc->size;
    Process** processes = tc->processes;
    int completed=0;
    int currentTime= 0;
    int idx = 0;

    // sort processes by arrival time, ascending
    sort(processes, processes +n, [](Process* a, Process*b){
        if (a->arrival != b->arrival){
            return a->arrival < b-> arrival;
        } else{
            return a->id < b->id;
        }
    });

    // creates a priority queue to store the processes by priority to simplify code
    priority_queue<Process*, vector<Process*>, _PrioComp> ready_queue;

    Process* prev = nullptr;
    int blockStart = 0;

    while (completed < n){
        // keep adding to the ready_queue all processes that have arrived.
        while(idx<n && processes[idx]->arrival <= currentTime){
            ready_queue.push(processes[idx]);
            idx++;
        }

        // once you have emptied the ready queue, jump to the next process
        if (ready_queue.empty()){
            prev = nullptr;
            blockStart = processes[idx]->arrival;
            currentTime = processes[idx]->arrival;
            continue;  
        
        }
        // from the ready queue, get the highest prio
        Process* p = ready_queue.top();
        ready_queue.pop();

        // calculate start and response time (for newly processed processes)
        if(p->start_time == -1){
            p->start_time = currentTime;
        }

        // if context switch is detected, print the current block and reset it
        if(prev != nullptr && prev != p){
            cout << blockStart << " "<< prev -> id <<" "<<  currentTime - blockStart << endl;
            blockStart = currentTime;
        }

        // run for 1 tick
        p->remaining --;
        currentTime ++;

        // if the process is done, print and reset. else, push it back so that it could be
        // reconsidered by the priority queue.
        if (p->remaining == 0){
            cout << blockStart <<" "<<  p -> id <<" "<<  currentTime - blockStart << 'X' << endl;
            p->completion_time = currentTime;
            completed++;
            prev = nullptr;
            blockStart = currentTime;
        } else {
            ready_queue.push(p);
            prev = p;
        }
    }
    // sort it back by id order
    sort(processes, processes + n, [](Process* a, Process* b){
    return a->id < b->id;
    });
    printResults(testNumber,tc);
}

void runRoundRobin(int testNumber, TestCase* tc){
    cout << testNumber << " " << tc->algorithm << endl;
    int n = tc->size;
    Process** processes = tc->processes;
    int quantum = tc->quantum;

    int completed = 0;
    int currentTime = 0;
    int idx = 0;

    //Sort Processes by arrival time for easy processing 
    sort(processes, processes + n, [](Process* a, Process* b){
        if (a->arrival != b->arrival){
            return a->arrival < b->arrival;
        }
        return a->id < b->id;
    });

    deque<Process*> readyQueue;

    while (completed < n){
        // Add all newly arrived processes to the front 
        // maintain arrival order among new arrivals 
        vector<Process*> newArrivals;
        while (idx < n && processes[idx]->arrival <= currentTime){
            newArrivals.push_back(processes[idx]);
            idx++;
        }

        //Add new Arrivals to front (priority over waiting processes)
        for (int i = newArrivals.size() - 1; i >= 0; i--){
            readyQueue.push_front(newArrivals[i]);
        }

        //if queue is empty, CPI is idle - jump to next arrival
        if (readyQueue.empty()) {
            currentTime = processes[idx]->arrival;
            continue;
        }

        //Get next process from Front of queue
        Process* p = readyQueue.front();
        readyQueue.pop_front();

        // Set start time if first time running 
        if (p->start_time == -1){
            p->start_time = currentTime;
        }

        //Calculate how long this process will run
        int runTime = min(quantum, p->remaining);
        p->remaining -= runTime;

        //determine if process is complete
        bool finished = (p->remaining == 0);

        //output Gantt chart entry
        cout << currentTime << " " << p->id << " " << runTime;
        if (finished){
            cout << "X";
            p->completion_time = currentTime + runTime;
            completed++;
        }

        cout << endl;

        //Update current time
        currentTime += runTime;

        // Check for processes that arrived during this execution because these will be added in the next iteration 
        //To ensure priority over the currently preempted process 

        //If process is not finished we add it to the BACK to the queue
        //Because preempted processes goes to the back
        if (!finished) {
            //check for new arrivals
            while (idx < n && processes[idx]->arrival <= currentTime){
                readyQueue.push_front(processes[idx]);
                idx++;
            }
            //add preempted process to back
            readyQueue.push_back(p);
        }
    }

    sort(processes, processes + n, [](Process* a, Process* b){
        return a->id < b->id;
    });

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

        //Read time quantum if Round Robin
        if (algorithm == "RR"){
            cin >> testCase->quantum;
        }
        
        // Iterates through each process per test case
        for (int j = 0; j < num_process; j++){
            int arrival, burst, nice;
            cin >> arrival >> burst >> nice;
            Process* proc = new Process(j+1, arrival, burst, nice);
            testCase->addProcess(j, proc);
        }
        // Temporary print function to keep track of processes
        testCase->printTestCase(i + 1);
        if (algorithm == "FCFS"){
            runFCFS(i+1, testCase);
        }
        else if (algorithm == "SRTF"){
            runSRTF(i+1, testCase);
        } else if (algorithm == "SJF"){
            runSJF(i+1,testCase);
        } else if (algorithm == "P"){
            runPrio(i+1,testCase);
        } 
        else if (algorithm == "RR"){
            runRoundRobin(i+1, testCase);
        }
    }
    
    return 0;
}