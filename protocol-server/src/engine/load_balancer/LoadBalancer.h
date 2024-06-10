#pragma once
#include "LoadBalancerStrategy.h"

// Context class that uses the load balancing strategy
template <class T>
class LoadBalancer {
private:
    LoadBalancerStrategy<T>* strategy;
    std::vector<T> servers;

public:
    explicit LoadBalancer<T>(LoadBalancerStrategy<T>* strategy) : strategy(strategy) {}

    void addServer(T server) {
        servers.push_back(server);
    }

    void removeAllServers() {
        servers.clear();
    }

    T requestServer() {
        return strategy->selectServer(servers);
    }
};
