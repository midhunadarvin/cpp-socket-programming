#pragma once
#include <any>
#include "LoadBalancerStrategy.h"

// Concrete implementation of a round-robin load balancing strategy
template <class T>
class RoundRobinStrategy : public LoadBalancerStrategy<T> {
public:
    T selectServer(const std::vector<T>& servers) const override {
        static int currentIndex = -1;
        currentIndex = (currentIndex + 1) % servers.size();
        return servers[currentIndex];
    }
};
