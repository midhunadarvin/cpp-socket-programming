#include <connection-pool/connection.h>
#include <connection-pool/pool.h>

using namespace cpool;

class TestConnection final : public Connection {
public:
    bool heart_beat() override { return connected; }

    bool is_healthy() override { return connected; }

    bool connect() override {
        connected = true;
        return connected;
    }

    void disconnect() override { connected = false; }

private:
    TestConnection() = default;
    friend ConnectionPoolFactory< TestConnection >;
    bool connected = false;
};

template <>
class ConnectionPoolFactory< TestConnection > {
public:
    static std::unique_ptr< ConnectionPool > create( const std::uint16_t num_connections ) {
        std::vector< std::unique_ptr< Connection > > connections;
        for ( std::uint16_t k = 0; k < num_connections; ++k ) {
            // cannot use std::make_unique, because constructor is hidden
            connections.emplace_back( std::unique_ptr< TestConnection >( new TestConnection{} ) );
        }
        return std::unique_ptr< ConnectionPool >( new ConnectionPool{std::move( connections )} );
    }
};