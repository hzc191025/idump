/*
 * main.cpp
 *
 *  Created on: 2015年5月17日
 *      Author: hzc
 */
#include "arpa/inet.h"

#include "pcap/pcap.h"
#include "pcap-int.h"

#include "pcap_linktypes.h"
#include "crctable.h"

#define PCAP_BUF_SIZE 2048

struct opt_config{
	int linktype;
	int pkt_cnt;
	int crcerr_cnt;
}opt;

typedef struct{
	u_int32_t msgcode;
	u_int32_t msglen;
}LinkLayer80211PrismHeader;

typedef struct{
	u_int32_t did;
	u_int16_t status;
	u_int16_t len;
	u_int32_t data;
}LinkLayer80211PrismItem;

void process_packet(u_char *user, const struct pcap_pkthdr *h, const u_char *bytes)
{
//	printf("sec=%d, usec=%06d, caplen=%d, len=%d\n", h->ts.tv_sec, h->ts.tv_usec, h->caplen, h->len);

	opt.pkt_cnt++;

	if (opt.linktype == LINKTYPE_IEEE802_11_PRISM){
		LinkLayer80211PrismHeader* hdr = (LinkLayer80211PrismHeader*)bytes;
		int hdrlen = ntohl(hdr->msglen);
		int datalen = ntohl(*(int*)(bytes+hdrlen-4));

		// calc crc
		unsigned int long crc;
		int n=0;
		const u_char* text = bytes+hdrlen;
		int calc_len = datalen - 4;
		crc = 0xFFFFFFFF;

		for( n = 0; n < calc_len; n++ )
			crc = crc_tbl[(crc ^ text[n]) & 0xFF] ^ (crc >> 8);
		crc = ~crc;

		if (crc != *((int*)(text+calc_len))){
			opt.crcerr_cnt++;
		}

		printf("header len = %d, data len = %4d, %08x, %02x%02x%02x%02x, cnt=%d, crcerr=%d\n",
				hdrlen, datalen, crc, text[datalen-1], text[datalen-2], text[datalen-3], text[datalen-4], opt.pkt_cnt, opt.crcerr_cnt);
	}
	else if (opt.linktype == LINKTYPE_IEEE802_11_RADIOTAP){

	}
}

int main(int argc, char* argv[])
{
	if (argc == 1){
		printf("%s %s\n", argv[0], " --help");
		return -1;
	}

	pcap_t *fp;
	char errbuf[PCAP_ERRBUF_SIZE];
	char source[PCAP_BUF_SIZE];

	pcap_t * file_descr = pcap_open_offline(argv[1], errbuf);
	if (file_descr == NULL){
		printf("failed to open %s\n");
		return -1;
	}

	opt.pkt_cnt = 0;
	opt.linktype = file_descr->linktype;
	opt.crcerr_cnt = 0;

	pcap_loop(file_descr, -1, process_packet, NULL);
	pcap_close(file_descr);

	printf("linktype = %d\n", file_descr->linktype);
	if (file_descr->linktype == LINKTYPE_IEEE802_11_PRISM){
		printf("LINKTYPE_IEEE802_11_PRISM\n");
	}

	printf("total:%d, crcerr=%d, %.2f%\n", opt.pkt_cnt, opt.crcerr_cnt, 100.0*opt.crcerr_cnt/opt.pkt_cnt);

	return 0;
}



