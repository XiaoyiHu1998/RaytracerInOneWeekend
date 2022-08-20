#ifndef WORK_COUNTER_HPP
#define WORK_COUNTER_HPP

class workCounter{
private:
    std::atomic<int> workDone;
    int totalWork;
    int threadCount;
    int image_height;

public:
    workCounter(int image_height, int threadCount):
        image_height{image_height},
        threadCount{threadCount},
        totalWork{image_height * threadCount}
    {
        std::atomic_init(&workDone, 0);
    }

    void incrementWork(int increments = 1){
        std::atomic_fetch_add(&workDone, increments);
    }

    void outputWorkDone(){
        std::ostringstream outputLine;
        int workDoneNonAtomic = std::atomic_load(&workDone);
        outputLine << "\rmain image progress: " << workDoneNonAtomic << "/" << totalWork << " | " << std::setprecision(2) << (double(workDoneNonAtomic) / double(totalWork)) * 100.0 << "%     "; 
        std::cerr << outputLine.str() << std::flush;
    }

    bool isWorkDone(){
        return std::atomic_load(&workDone) == totalWork;
    }

};

#endif //WORK_COUNTER_HPP