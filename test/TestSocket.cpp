#include "Infra/Network/Socket.h"

int main()
{
    auto socket = Infra::Socket::Create().value();
    Infra::IpAddress target = Infra::IpAddress::TryParse("10.12.16.56").value();
    Infra::EndPoint endpoint(target, 4869);

}