#pragma once

#include <sstream>
#include <string>
#include <chrono>
#include <map>
#include <vector>

//
// To enable profiling define VERY_SIMPLE_PROFILER
//
// Usage example:
//
// #define VERY_SIMPLE_PROFILER
// #include "verySimpleProfiler.hpp"
//
// void some_function()
// {
//     VERY_SIMPLE_FUNC_PROFILE();

//     {
//         VERY_SIMPLE_BLOCK_PROFILE("Calculating 1");
//         // some calculations
//     }
//     // some other processing
//     {
//         VERY_SIMPLE_BLOCK_PROFILE("Calculating 2");
//         // some calculations
//     }
// }

namespace VerySimpleProfilerSpace 
{
//------------------------------------------------------------------------------
struct VerySimpleEntry
{
    std::string name;
    long long duration;
};

//------------------------------------------------------------------------------
class VerySimpleProfiler
{
    public:
        static VerySimpleProfiler& instance()
        {
            static VerySimpleProfiler profiler;
            return profiler;
        }

        ~VerySimpleProfiler()
        {
            saveProfilerStatistics(_entries, "very_simple_profiler.log");
        }

        // Add new profiler entry
        static void appendEntry(const VerySimpleEntry &entry)
        {
            VerySimpleProfiler &p = VerySimpleProfiler::instance();
            p._entries[entry.name].push_back(entry);
        }

        // Save profiler info to the file
        static void save(const std::string &filename)
        {
            VerySimpleProfiler &p = VerySimpleProfiler::instance();
            saveProfilerStatistics(p._entries, filename);
        }

    private:
        VerySimpleProfiler() = default;

        static void saveProfilerStatistics(const std::map<std::string, std::vector<VerySimpleEntry>> &entries, const std::string &filename)
        {
            time_t rawtime;
            struct tm * ltm;
            time(&rawtime);
            ltm = localtime(&rawtime);

            std::stringstream ss;
            ss << 1900 + ltm->tm_year << "-" << ltm->tm_mon + 1 << "-" << ltm->tm_mday << " "
                << ltm->tm_hour << ":" << ltm->tm_min << ":" << ltm->tm_sec << "\n";

            auto stat_info = [](const std::vector<VerySimpleEntry> &entriesVec)
            {
                double maxDuration = 0.;
                double minDuration = std::numeric_limits<double>::max();
                double avgDuration = 0.;
                unsigned int total = (unsigned int)entriesVec.size();

                for (const VerySimpleEntry &e : entriesVec) {
                    double d = (double)e.duration;
                    avgDuration += d;
                    if (d > maxDuration) {
                        maxDuration = d;
                    }
                    if (d < minDuration) {
                        minDuration = d;
                    }
                }
                avgDuration /= (double)total;
                return std::make_tuple(total, maxDuration, minDuration, avgDuration);
            };

            for (const auto &e : entries) {
                const auto &stats = stat_info(e.second);
                ss << e.first << " (" << std::get<0>(stats)
                    << ") Max = " << std::get<1>(stats)
                    << " Min = " << std::get<2>(stats)
                    << " Avg = " << std::get<3>(stats) << "\n";
            } // for (const auto &e : entries)

            FILE * fp = fopen(filename.c_str(), "a");
            fprintf(fp, "%s\n", ss.str().c_str());
            fflush(fp);
            fclose(fp);
        }

    private:
        std::map<std::string, std::vector<VerySimpleEntry>> _entries;
        
};

//------------------------------------------------------------------------------
struct VerySimpleBlock 
{
    std::string name; // Profiler block name
    std::chrono::high_resolution_clock::time_point startTime; // Starting time of the block

    VerySimpleBlock(const std::string &blockName)
        : name(blockName)
    {
        startTime = std::chrono::high_resolution_clock::now();
    }
    ~VerySimpleBlock()
    {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        if (elapsedTime.count() > 0) {
            VerySimpleProfiler::appendEntry({ name, elapsedTime.count() });
        }
    }
};

}

#define STRINGIZE_DETAIL_(v)	#v
#define STRINGIZE(v) STRINGIZE_DETAIL_(v)
#define FUNCTION_STRING	STRINGIZE(__FUNCTION__)
#define BLOCK_NAME FUNCTION_STRING

#ifdef VERY_SIMPLE_PROFILER
#define VERY_SIMPLE_FUNC_PROFILE() VerySimpleProfilerSpace::VerySimpleBlock verySimpleProfilerBlock##__LINE__(__FUNCTION__);
#define VERY_SIMPLE_BLOCK_PROFILE(x) VerySimpleProfilerSpace::VerySimpleBlock verySimpleProfilerBlock##__LINE__(x);
#else
#define VERY_SIMPLE_FUNC_PROFILE() c
#define VERY_SIMPLE_BLOCK_PROFILE(x)
#endif