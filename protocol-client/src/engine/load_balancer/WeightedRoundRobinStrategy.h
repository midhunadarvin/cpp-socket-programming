#pragma once
#include <any>
#include "LoadBalancerStrategy.h"

class WeightedItem {
public:
    int weight;
    WeightedItem(int weight): weight(weight) {}
};
// Concrete implementation of a weighted round-robin load balancing strategy
template <class T, typename = std::enable_if<std::is_base_of<WeightedItem, T>::value>>
class WeightedRoundRobinStrategy : public LoadBalancerStrategy<T> {
public:
    T selectServer(const std::vector<T>& servers) const override {
        static int currentIndex = -1;
        static int weight = 0;

        if (weight > 0) {
            weight--;
        }

        if (weight == 0) {
            currentIndex = (currentIndex + 1) % servers.size();
            weight = servers[currentIndex].weight;
        }
        return servers[currentIndex];
    }
};
