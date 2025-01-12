#pragma once

#include <Interpreters/Context_fwd.h>
#include <Server/HTTP/HTTPRequestHandler.h>

#include <Poco/Logger.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include <Poco/Data/SessionPool.h>
#pragma GCC diagnostic pop

#include <mutex>
#include <unordered_map>


namespace DB
{
/** Main handler for requests to ODBC driver
  * requires connection_string and columns in request params
  * and also query in request body
  * response in RowBinary format
  */
class ODBCHandler : public HTTPRequestHandler, WithContext
{
public:
    using PoolPtr = std::shared_ptr<Poco::Data::SessionPool>;
    using PoolMap = std::unordered_map<std::string, PoolPtr>;

    ODBCHandler(std::shared_ptr<PoolMap> pool_map_,
        size_t keep_alive_timeout_,
        ContextPtr context_,
        const String & mode_)
        : WithContext(context_)
        , log(&Poco::Logger::get("ODBCHandler"))
        , pool_map(pool_map_)
        , keep_alive_timeout(keep_alive_timeout_)
        , mode(mode_)
    {
    }

    void handleRequest(HTTPServerRequest & request, HTTPServerResponse & response) override;

private:
    Poco::Logger * log;

    std::shared_ptr<PoolMap> pool_map;
    size_t keep_alive_timeout;
    String mode;

    static inline std::mutex mutex;

    PoolPtr getPool(const std::string & connection_str);
    void processError(HTTPServerResponse & response, const std::string & message);
};

}
