
#if defined (__BTMTK__) && defined (__BT_DUN_PROFILE__)

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <netdb.h>
#include <linux/if.h>
#include <netinet/in.h>
#include <linux/if_arp.h>

#include "pppd.h"
#include "fsm.h"
#include "ipcp.h"
#include "btdunutils.h"


static int pppbt_setsockname(char **argv) ;
static int pppbt_setipaddr(char **argv);
static int pppbt_connect(void);
static void pppbt_disconnect(void) ;
static void pppbt_check_option(void);
//static void getipaddr(char *ifname, char *ipaddr);


static option_t pppbt_options[] = {
    {"pppbt-setsockname", o_special, pppbt_setsockname, "PPPBT set socket name", OPT_DEVNAM, NULL, 0, 0, NULL, 0, 0},
    {"pppbt-setipaddr", o_special, pppbt_setipaddr, "PPPBT set ip address", OPT_DEVNAM, NULL, 0, 0, NULL, 0, 0},
    {NULL, 0, NULL, NULL, 0, NULL, 0, 0, NULL, 0, 0},
};

static struct channel pppbt_channel = {
    .options = pppbt_options,
    .process_extra_options = NULL,
    .check_options = pppbt_check_option,
    .connect = pppbt_connect,
    .disconnect = pppbt_disconnect,
    .establish_ppp = generic_establish_ppp,
    .disestablish_ppp = generic_disestablish_ppp,
    .send_config = NULL,
    .recv_config = NULL,
    .cleanup = NULL,
    .close = NULL,
};

int pppbt = -1;
static char *pppbt_sock_path;

bool bPppbt = 0;
 
#define PPPBT_IP_ADDR_LEN		15



void pppbt_init() 
{
    add_options(pppbt_options);
}

void pppbt_deinit()
{
    bPppbt = 0;
}

/* Set socket path name */
static int pppbt_setsockname(char **argv) 
{
    struct protent *protp= &ipcp_protent;
    printf("pppbt_set argv len: %d\n", strlen(*argv));

    // "bt.ext.adp.spp.data"
    pppbt_sock_path = (char *)malloc(strlen(*argv) + 1);
    if (!pppbt_sock_path)
    {
        return -1;
    }
    memcpy(pppbt_sock_path, *argv, strlen(*argv));
    info("path: %s\n", pppbt_sock_path);

    the_channel = &pppbt_channel;
    bPppbt = 1;

    protp->init(0);
    
    return 1;
}

static int pppbt_setipaddr(char **argv)
{
    //char ipaddr[PPPBT_IP_ADDR_LEN + 1] = "192.168.3.2";
    char ipaddr[PPPBT_IP_ADDR_LEN + 1];
    char *addr;
    char *ch;

    info("pppbt_setipaddr: %s\n", *argv);

    //getipaddr(*argv, ipaddr); 
    memset(ipaddr, 0, sizeof(ipaddr));
    if (ip_dun_create_netdev(*argv, ipaddr) < 0)
    {
        return -1;
    }

    addr = (char *)malloc(strlen(ipaddr) + 2);
    if (addr == NULL)
    {
        return -1;
    }
    memset(addr, 0, strlen(ipaddr) + 2);
    memcpy(addr, ipaddr, strlen(ipaddr));
   
    ch = addr + strlen(ipaddr);
    *ch = ':';

    info("ip addr: %s", addr);	
    return setipaddr(addr, NULL, 1);
}

static int pppbt_connect(void) 
{
    struct sockaddr_un btaddr;
    socklen_t btaddrlen;

    pppbt = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (pppbt < 0)
    {
        info("[pppbt] create bt data socket failed\n");
        return -1;
    }
    
    btaddr.sun_family = AF_LOCAL;
    btaddr.sun_path[0] = 0;	
    strcpy(btaddr.sun_path + 1, pppbt_sock_path);
    btaddrlen = offsetof(struct sockaddr_un, sun_path) + strlen(btaddr.sun_path) + 1;	

    if (connect(pppbt, (struct sockaddr *)&btaddr, btaddrlen) < 0)
    {
        info("[pppbt] connect to bt.ext.adp.spp.data failed: %s(%d)\n", strerror(errno), errno);
        return -1;
    }

    //if (ip_dun_init(pppbt) < 0)
        //return -1;
    ip_dun_setfd(pppbt);

    return pppbt;
}

/* Close Bluetooth DUN socket */
static void pppbt_disconnect(void) 
{
    info("[pppbt] disconnect");
    if (pppbt >= 0) {
        close(pppbt);
        pppbt = -1;
    }
    ip_dun_remove_netdev();
}

/* Set global parameters */
static void pppbt_check_option(void)
{
    nodetach = 1;
}

#endif
