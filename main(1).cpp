#include <iostream>
#include <functional>
#include <Ws2tcpip.h>
#include <ifmib.h>
#include <ipmib.h>
typedef std::function<void(void)> callback;

struct trival {
	int port;
	bool able;
	callback m_cb;
public:
	trival(int port, bool able, callback cb = NULL);
};

trival::trival(int port, bool able, callback cb)
	: port(port)
	, able(able)
	, m_cb(cb)
{
}

std::vector<ip_route> Upnp::__getRoutes(int &ec)
{
    std::vector<ip_route> ret;

    // Load Iphlpapi library
    HMODULE iphlp = LoadLibraryA("Iphlpapi.dll");
    if (!iphlp)
    {
        ec = -1;
        return std::vector<ip_route>();
    }

    typedef DWORD(WINAPI *GetIfEntry_t)(PMIB_IFROW pIfRow);
    GetIfEntry_t GetIfEntry = (GetIfEntry_t)GetProcAddress(iphlp, "GetIfEntry");
    if (!GetIfEntry)
    {
        ec = -1;
        return std::vector<ip_route>();
    }

    // Get GetIpForwardTable() pointer
    typedef DWORD(WINAPI *GetIpForwardTable_t)(PMIB_IPFORWARDTABLE pIpForwardTable, PULONG pdwSize, BOOL bOrder);

    GetIpForwardTable_t GetIpForwardTable = (GetIpForwardTable_t)GetProcAddress(
        iphlp, "GetIpForwardTable");
    if (!GetIpForwardTable)
    {
        FreeLibrary(iphlp);
        ec = -1;
        return std::vector<ip_route>();
    }

    MIB_IPFORWARDTABLE *routes = NULL;
    ULONG out_buf_size = 0;
    if (GetIpForwardTable(routes, &out_buf_size, FALSE) != ERROR_INSUFFICIENT_BUFFER)
    {
        FreeLibrary(iphlp);
        ec = -1;
        return std::vector<ip_route>();
    }

    routes = (MIB_IPFORWARDTABLE *)malloc(out_buf_size);
    if (!routes)
    {
        FreeLibrary(iphlp);
        ec = -1;
        return std::vector<ip_route>();
    }

    if (GetIpForwardTable(routes, &out_buf_size, FALSE) == NO_ERROR)
    {
        for (int i = 0; i < routes->dwNumEntries; ++i)
        {
            ip_route r;
            nanoev_addr_init(&r.destination, inet_ntoa(*(in_addr*)&routes->table[i].dwForwardDest), 0);
            nanoev_addr_init(&r.netmask, inet_ntoa(*(in_addr*)&routes->table[i].dwForwardMask), 0);
            nanoev_addr_init(&r.gateway, inet_ntoa(*(in_addr*)&routes->table[i].dwForwardNextHop), 0);

            MIB_IFROW ifentry;
            ifentry.dwIndex = routes->table[i].dwForwardIfIndex;
            if (GetIfEntry(&ifentry) == NO_ERROR)
            {
                wcstombs(r.name, ifentry.wszName, sizeof(r.name));
                r.name[sizeof(r.name) - 1] = 0;
                r.mtu = ifentry.dwMtu;
                ret.push_back(r);
            }
        }
    }

    // Free memory
    free(routes);
    FreeLibrary(iphlp);

    return ret;
}

int main()
{
	std::shared_ptr<trival> ptr;
	ptr.reset(new trival(1, true));
	std::cout << "p. port = " << ptr->port;
	
}

