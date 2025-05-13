//
// Created by Administrator on 2025/5/11.
//

#ifndef SCOPETIMER_H
#define SCOPETIMER_H
#include "OutStream.h"
#include "../ThirdParty.h"
namespace FCT
{
    class ScopeTimer {
    private:
        std::string name;
        std::chrono::high_resolution_clock::time_point startTime;
        bool stopped;
        static std::map<std::string, double> timings;
    public:
        ScopeTimer(const std::string& timerName)
            : name(timerName), stopped(false) {
            startTime = std::chrono::high_resolution_clock::now();
        }

        ~ScopeTimer() {
            if (!stopped) {
                stop();
            }
        }

        void stop() {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();

            timings[name] = duration;
            stopped = true;
        }

        static double getTime(const std::string& timerName) {
            auto it = timings.find(timerName);
            if (it != timings.end()) {
                return it->second;
            }
            return 0.0;
        }
        static void printAll(std::ostream& out = fout) {
            out << "===== ScopeTimer Results =====\n";
            for (const auto& timing : timings) {
                out << timing.first << ": " << timing.second << " us\n";
            }
            out << "============================\n";
        }
    };
} // namespace FCT
#define FCTSCOPETIMER(name) FCT::ScopeTimer name##Timer(#name)
#endif //SCOPETIMER_H
