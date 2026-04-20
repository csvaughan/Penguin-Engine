#pragma once
#include <random>
#include <type_traits>
#include <thread>
#include <atomic>
#include <vector>     
#include <algorithm>  
#include <initializer_list>

namespace pgn
{
    class Random
    {
    public:
        Random() = delete;

        static inline std::atomic<unsigned int> masterSeed{ std::random_device{}() };

        static void SetGlobalSeed(unsigned int seed) { masterSeed.store(seed); }

        static std::mt19937& GetEngine() 
        {
            static thread_local std::mt19937 engine(
                masterSeed.load() + 
                static_cast<unsigned int>(std::hash<std::thread::id>{}(std::this_thread::get_id()))
            );
            return engine;
        }

        // --- Core Functions ---

        template <typename T>
        static T Uniform(T min, T max) 
        {
            if constexpr (std::is_floating_point_v<T>) 
            {
                return std::uniform_real_distribution<T>(min, max)(GetEngine());
            }
            else
            {
                using DistType = std::conditional_t<sizeof(T) < 2, int, T>;
                
                return static_cast<T>(
                    std::uniform_int_distribution<DistType>(
                        static_cast<DistType>(min), 
                        static_cast<DistType>(max)
                    )(GetEngine())
                );
            }
        }

        template <typename T>
        static T Normal(T mean, T stddev) { return std::normal_distribution<T>(mean, stddev)(GetEngine()); }

        // --- Helper Functions ---

        static bool Chance(float probability) { return Uniform(0.0f, 1.0f) < probability; }

        static int Roll(int sides) { return Uniform(1, sides); }

        static int Sign() { return Chance(0.5f) ? 1 : -1; }

        template <typename T>
        static void Shuffle(std::vector<T>& vec) 
        { 
            std::shuffle(vec.begin(), vec.end(), GetEngine());
        }

        static size_t WeightedIndex(const std::vector<double>& weights) 
        {
            std::discrete_distribution<size_t> dist(weights.begin(), weights.end());
            return dist(GetEngine());
        }

        static size_t WeightedIndex(std::initializer_list<double> weights) 
        {
            std::discrete_distribution<size_t> dist(weights);
            return dist(GetEngine());
        };
    };
}