#include "ns.h"
#include "kern/e1000.h"

extern union Nsipc nsipcbuf;
static struct jif_pkt *pkt = (struct jif_pkt*)&nsipcbuf;

	void
static msleep(int msec)
{
	unsigned now = sys_time_msec();
	unsigned end = now + msec;

	if ((int)now < 0 && (int)now > -MAXERROR)
		panic("sys_time_msec: %e", (int)now);
	if (end < now)
		panic("sleep: wrap");

	while (sys_time_msec() < end)
		sys_yield();
}


	void
input(envid_t ns_envid)
{
	binaryname = "ns_input";

	// LAB 6: Your code here:
	// 	- read a packet from the device driver
	//	- send it to the network server
	// Hint: When you IPC a page to the network server, it will be
	// reading from it for a while, so don't immediately receive
	// another packet in to the same physical page.

	int r;
	if ((r = sys_page_alloc(0, pkt, PTE_P|PTE_U|PTE_W)) < 0)
		panic("sys_page_alloc: %e", r);


	envid_t current_envid = sys_getenvid();
	while (1) {

		//cprintf("Reading a packet from the device driver\n");

		int length;

		while((r=sys_net_try_receive(pkt->jp_data, &length))<0) {
			msleep(100);
		}

		nsipcbuf.pkt.jp_len = length;

		//cprintf("Received a packet from the device driver, data = %s, length = %d\n", nsipcbuf.pkt.jp_data, nsipcbuf.pkt.jp_len);

		//cprintf("Sending a packet to the network server in environment....%0x\n", current_envid);
		ipc_send(ns_envid, NSREQ_INPUT, pkt, PTE_P|PTE_W|PTE_U);
		//cprintf("packet sent to the network server\n");
		msleep(100);
	}
}
