
#ifndef PROXYFMU_REMOTE_HPP
#define PROXYFMU_REMOTE_HPP

#include <string>

namespace proxyfmu
{

struct remote
{
    const std::string host;
    const int port;

    remote(const std::string& host, int port)
        : host(host)
        , port(port)
    { }

};

} // namespace proxyfmu

#endif //PROXYFMU_REMOTE_HPP
