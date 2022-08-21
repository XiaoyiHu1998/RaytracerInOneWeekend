#ifndef WORK_COUNTER_HPP
#define WORK_COUNTER_HPP

//TODO: implement seperate workDones
class workCounter{
private:
    std::atomic<int> workDoneMain;
    std::atomic<int> workDoneAlbedo;
    std::atomic<int> workDoneNormal;
    int totalWork;
    int threadCount;
    int image_height;

public:
    workCounter(int image_height, int threadCount):
        image_height{image_height},
        threadCount{threadCount},
        totalWork{image_height * threadCount}
    {
        std::atomic_init(&workDoneMain, 0);
        std::atomic_init(&workDoneAlbedo, 0);
        std::atomic_init(&workDoneNormal, 0);
    }

    void incrementWorkMain(int increments = 1){
        std::atomic_fetch_add(&workDoneMain, increments);
    }

    void incrementWorkAlbedo(int increments = 1){
        std::atomic_fetch_add(&workDoneAlbedo, increments);
    }

    void incrementWorkNormal(int increments = 1){
        std::atomic_fetch_add(&workDoneNormal, increments);
    }

    void outputWorkDone(){
        std::ostringstream outputLine;
        int workDoneMainNonAtomic = std::atomic_load(&workDoneMain);
        int workDoneAlbedoNonAtomic = std::atomic_load(&workDoneAlbedo);
        int workDoneNormalNonAtomic = std::atomic_load(&workDoneNormal);
        int totalWorkDoneNonAtomic = workDoneAlbedoNonAtomic + workDoneNormalNonAtomic + workDoneMainNonAtomic;

        outputLine << "\rTotal: " << std::setprecision(2) << (double(totalWorkDoneNonAtomic) / double(totalWork * 3)) * 100.0 << "%" 
        << " | "
        << "Main:" << workDoneMainNonAtomic << "/" << totalWork
        << " | "
        << "Albedo: " << workDoneAlbedoNonAtomic << "/" << totalWork
        << " | "
        << "Normal: " << workDoneNormalNonAtomic << "/" << totalWork;

        std::cerr << outputLine.str() << std::flush;
    }

    bool isWorkDone(){
        return std::atomic_load(&workDoneMain) == totalWork && std::atomic_load(&workDoneAlbedo) == totalWork && std::atomic_load(&workDoneNormal) == totalWork;
    }

};

#endif //WORK_COUNTER_HPP