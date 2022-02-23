#ifndef BENCHMARK_HPP
#define BENCHMARK_HPP

#define BENCH_SAVE_PATH "benchmarks/"

#include <GLFW/glfw3.h>

#include <functional>
#include <map>
#include <string>
#include <vector>

class Engine;


typedef int BenchFlags;

enum BenchFlags_
{
    BenchFlags_Meteorite = 1 << 0,
    BenchFlags_City = 1 << 1,
    BenchFlags_LowTests = 1 << 2,
    BenchFlags_MediumTests = 1 << 3,
    BenchFlags_HighTests = 1 << 4,
    BenchFlags_SaveCsv = 1 << 6,
    BenchFlags_ShowResults = 1 << 7
};

class Benchmark
{
  private:
    // "test_name" , vector of delta_time
    std::map<std::string, std::vector<double>> results;
    std::vector<std::function<bool()>> benchFuns = {};
    bool finishedSignal = false;

    int benchStep = 0;
    bool saveResults = false;
    inline static float moveSpeed = 1.0;

    Engine *engine;

    bool BNCH_asteroidLow();
    bool BNCH_asteroidMedium();
    bool BNCH_asteroidHigh();

    bool BNCH_cityLow();
    bool BNCH_cityMedium();
    bool BNCH_cityHigh();
    
    void saveAsCsv();

  public:
    Benchmark(Engine *_engine, BenchFlags _flags);
    //! execute a benchmark loop
    void exec();

    bool isFinished() { return finishedSignal; }
};

#endif