#include "ns.h"

extern union Nsipc nsipcbuf;

void
output(envid_t ns_envid)
{
	binaryname = "ns_output";

	// LAB 6: Your code here:
	// 	- read a packet from the network server
	//	- send the packet to the device driver
	int r;
	envid_t current_envid = sys_getenvid();
        while (1) {
		//cprintf("Waiting to receive in environment....%0x\n\n", current_envid);
                r = sys_ipc_recv(&nsipcbuf);
		//cprintf("packet received from network server\n");

                if ((thisenv->env_ipc_from != ns_envid) ||
                    (thisenv->env_ipc_value != NSREQ_OUTPUT)) {
                        continue;
                }
		//cprintf("Reaching here....received right request from the network server, data = %s\n", nsipcbuf.pkt.jp_data);

                while ((r = sys_net_try_send(nsipcbuf.pkt.jp_data,
                                             nsipcbuf.pkt.jp_len)) != 0) {
			cprintf("sys_net_try_send(), r = %d\n", r);
		}

        }

}
