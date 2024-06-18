#pragma once
#include "LoadBalancerStrategy.h"

// Concrete implementation of a random load balancing strategy
template <class T>
class RandomStrategy : public LoadBalancerStrategy<T> {
public:
    T selectServer(const std::vector<T>& servers) const override {
        return servers[rand() % servers.size()];
    }
};
