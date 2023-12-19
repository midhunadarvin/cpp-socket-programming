#include <connection-pool/pool.h>
#include <iostream>
#include <algorithm>

namespace cpool
{

    /*====================================================================================================================*/
    /* ConnectionPool::ConnectionProxy                                                                                    */
    /*====================================================================================================================*/

    ConnectionPool::ConnectionProxy::ConnectionProxy(ConnectionPool *pool, Connection *connection) noexcept
        : m_pool{pool}, m_connection{connection} {}

    ConnectionPool::ConnectionProxy::ConnectionProxy(ConnectionPool::ConnectionProxy &&other) noexcept
        : m_pool{other.m_pool}, m_connection{other.m_connection}
    {
        other.m_pool = nullptr;
        other.m_connection = nullptr;
    }

    ConnectionPool::ConnectionProxy &
    ConnectionPool::ConnectionProxy::operator=(ConnectionPool::ConnectionProxy &&other) noexcept
    {
        if (this == &other)
        {
            // prevent self-assignment
            return *this;
        }

        m_pool = other.m_pool;
        m_connection = other.m_connection;
        other.m_pool = nullptr;
        other.m_connection = nullptr;
        return *this;
    }

    ConnectionPool::ConnectionProxy::~ConnectionProxy()
    {
        std::cout << "ConnectionProxy::destructor" << std::endl;
        if (m_pool != nullptr)
        {
            m_pool->release_connection(m_connection);
        }
    }

    Connection *ConnectionPool::ConnectionProxy::operator->() { return m_connection; }

    Connection &ConnectionPool::ConnectionProxy::operator*() { return *m_connection; }

    bool ConnectionPool::ConnectionProxy::valid() const
    {
        if ((m_pool != nullptr) && (m_connection != nullptr))
        {
            std::unique_lock lock{m_pool->m_connections_mtx};
            return m_pool->m_connections_busy.count(m_connection) > 0;
        }

        return false;
    }

    /*====================================================================================================================*/
    /* ConnectionPool                                                                                                     */
    /*====================================================================================================================*/

    namespace
    {

        bool check_connect(Connection &connection)
        {
            try
            {
                std::cout << "check_connect" << std::endl;
                if (!connection.is_healthy())
                {
                    return connection.connect();
                }
            }
            catch (std::exception &e)
            {
                std::cout << e.what() << std::endl;
            }

            return true;
        }

    } // namespace

    ConnectionPool::ConnectionPool(std::vector<std::unique_ptr<Connection>> &&connections)
    {
        for (auto &connection : connections)
        {
            Connection *key = connection.get();
            m_connections_idle.emplace(key, std::move(connection));
        }
    }

    ConnectionPool::~ConnectionPool() = default;

    ConnectionPool::ConnectionProxy ConnectionPool::get_connection()
    {
        std::cout << "get_connection" << std::endl;
        std::unique_lock lock{m_connections_mtx};

        if (m_connections_idle.empty())
        {
            return ConnectionProxy{nullptr, nullptr};
        }

        int i = 0;
        for (auto &item : m_connections_idle)
        {
            i++;
            if (!check_connect(*item.second))
            {
                continue;
            }
            std::cout << "Got an idle connection :: " << i << std::endl;
            ConnectionProxy proxy{this, item.first};
            auto node = m_connections_idle.extract(item.first);
            m_connections_busy.insert(std::move(node));
            return proxy;
        }

        return ConnectionProxy{nullptr, nullptr};
    }

    void ConnectionPool::release_connection(ConnectionPool::ConnectionProxy &&proxy)
    {
        release_connection(proxy.m_connection);
    }

    void ConnectionPool::release_connection(Connection *connection)
    {
        std::cout << "release_connection" << std::endl;
        if (connection == nullptr)
        {
            return;
        }

        std::unique_lock lock{m_connections_mtx};
        if (auto it = m_connections_busy.find(connection); it != m_connections_busy.end())
        {
            // check_connect( *it->second );
            auto node = m_connections_busy.extract(it);
            m_connections_idle.insert(std::move(node));
        }
    }

    std::size_t ConnectionPool::size() const
    {
        std::unique_lock lock{m_connections_mtx};
        return m_connections_busy.size() + m_connections_idle.size();
    }

    std::size_t ConnectionPool::size_idle() const
    {
        std::unique_lock lock{m_connections_mtx};
        return m_connections_idle.size();
    }

    std::size_t ConnectionPool::size_busy() const
    {
        std::unique_lock lock{m_connections_mtx};
        return m_connections_busy.size();
    }

    void ConnectionPool::heart_beat()
    {
        std::unique_lock lock{m_connections_mtx};

        // Only send heart beat on idle connections,
        // busy connections should be busy for a reason.
        for (auto &connection : m_connections_idle)
        {
            connection.second->heart_beat();
        }
    }

} // namespace cpool