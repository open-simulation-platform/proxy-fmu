
#ifndef PROXYFMU_REMOTE_INFO_HPP
#define PROXYFMU_REMOTE_INFO_HPP

#include <string>
#include <utility>

namespace proxyfmu
{

struct remote_info
{
    const std::string host;
    const int port;

    remote_info(std::string host, int port)
        : host(std::move(host))
        , port(port)
    { }

};

} // namespace proxyfmu

#endif //PROXYFMU_REMOTE_INFO_HPP
