#pragma once
#include <iostream>
#include <vector>
#include <cstdlib>
#include <any>

// Abstract base class for load balancing strategies
template <class T>
class LoadBalancerStrategy {
public:
    virtual T selectServer(const std::vector<T>& servers) const = 0;
    virtual ~LoadBalancerStrategy() = default;
};
