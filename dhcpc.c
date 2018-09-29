#include <stdio.h> /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), sendto() and
recvfrom() */
#include <arpa/inet.h> /* for sockaddr_in and inet_addr() */
#include <stdlib.h> /* for atoi() and exit() */
#include <string.h> /* for memset() */
#include <unistd.h> /* for close() */
#include <net/if.h> /*for ifreq*/
#include <sys/socket.h>/*for setsockopt()*/
#include <time.h>
#include <sys/ioctl.h>


struct DHCPpacket Spacket;
struct DHCPpacket Rpacket;
unsigned int ServAddrLen; 
int sock,sock1; /* Socket descriptor */
struct sockaddr_in ServAddr; /*server address */
struct sockaddr_in ClntAddr;  /*client address */
struct sockaddr_in fromAddr;	/* Source address of echo */
unsigned int fromSize; /* In-out of address size for recvfrom() */
unsigned short ClientPort = 68;
unsigned short ServPort = 67; /*server port */
char broadcastIP[] = "255.255.255.255"; /* broadcast address */
int recvMsgSize; /* Length of received response */
time_t start,end;  
char comment;
int lease ;
char* abIP;
unsigned int IP;
char* IPPointer;



struct DHCPpacket{
	uint8_t OP;
	uint8_t HTYPE;
	uint8_t HLEN;
	uint8_t HOPS;
	uint32_t TID;
	uint16_t SEC;
	uint16_t FLAGS;
	uint32_t CIP;
	uint32_t YIP;
	uint32_t SIP;
	uint32_t RIP;
	uint8_t CHADDR[16];
	uint8_t SHNAME[64];
	uint8_t FNAME[128];
	uint32_t cookie;
	uint8_t Options[3];
	uint8_t RestOP[25];
} DHCP; 

unsigned int getIP()
{
    
 int sock;
    
 struct sockaddr_in sin;
    
 struct ifreq ifr;    
 if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        
 perror("socket");    
 strncpy(ifr.ifr_name, "eth1", IFNAMSIZ);
    
 ifr.ifr_name[IFNAMSIZ - 1] = 0;    
 if(ioctl(sock, SIOCGIFADDR, &ifr) < 0)
        
 perror("ioctl");     
 memcpy(&sin, &ifr.ifr_addr, sizeof(sin));    
 unsigned int IP = sin.sin_addr.s_addr;
    
 return IP;}

void SetPacket(char type){
	/*get mac address of client*/
	unsigned int cIP=getIP();
	struct ifreq ifr;
	int k; 
	unsigned char mac[6];  
	mac[0] = (unsigned char)ifr.ifr_hwaddr.sa_data[0];
	mac[1] = (unsigned char)ifr.ifr_hwaddr.sa_data[1];
	mac[2] = (unsigned char)ifr.ifr_hwaddr.sa_data[2];
	mac[3] = (unsigned char)ifr.ifr_hwaddr.sa_data[3];
	mac[4] = (unsigned char)ifr.ifr_hwaddr.sa_data[4];
	mac[5] = (unsigned char)ifr.ifr_hwaddr.sa_data[5];
	mac[5] = mac[5] && 0x0000ff;
	char curmacstr[64];
	memset(curmacstr,0,sizeof(curmacstr));
	sprintf(curmacstr,"%.2X:%.2X:%.2X:%.2X:%.2X:%.2X",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]); 
	/*generate packet*/
	switch (type){
		case '1':         /*DHCPDISCOVER*/
			Spacket.OP = 1;
			Spacket.HTYPE = 1;
			Spacket.HLEN = 6;
			Spacket.HOPS = 0;
			Spacket.TID = 0;
			Spacket.SEC = 0;
			Spacket.FLAGS = 0x0080;
			Spacket.CIP = 0;
			Spacket.YIP = 0;
			Spacket.SIP = 0;
			Spacket.RIP = 0;
			memcpy(&Spacket.CHADDR,&curmacstr,sizeof(curmacstr));
			for(k=0;k<10;k++){
				Spacket.CHADDR[6+k]= 0x00;
			}
			for(k=0;k<64;k++){
				Spacket.SHNAME[k]=0x00;
			}
			for(k=0;k<128;k++){
				Spacket.FNAME[k]=0x00;
			}
			Spacket.cookie =0x63538263;
			Spacket.Options[0] = 0x35;
			Spacket.Options[1] = 0x01;
			Spacket.Options[2] = 0x01;
			Spacket.RestOP[0] = 0xff;
			break;
		case '2':		/*DHCPREQUEST*/
			Spacket.OP = 1;
			Spacket.HTYPE = 0x01;
			Spacket.HLEN = 6;
			Spacket.HOPS = 0;
			Spacket.TID = 0;
			Spacket.SEC = 0;
			Spacket.FLAGS = 0;
			Spacket.CIP = Rpacket.CIP;
			Spacket.YIP = 0;
			Spacket.SIP = Rpacket.SIP;
			Spacket.RIP = 0;
			memcpy(&Spacket.CHADDR,&curmacstr,sizeof(curmacstr));
			for(k=0;k<10;k++){
				Spacket.CHADDR[6+k]= 0x00;
			}
			for(k=0;k<64;k++){
				Spacket.SHNAME[k]=0x00;
			}
			for(k=0;k<128;k++){
				Spacket.FNAME[k]=0x00;
			}
			Spacket.cookie =0x63538263;
			Spacket.Options[0] = 0x35;
			Spacket.Options[1] = 0x01;
			Spacket.Options[2] = 0x03;
			Spacket.RestOP[0] = 0xff;
			if (comment == '3')
				Spacket.CIP = inet_addr(abIP);
			break;
		case '3':	/*DHCPRELEASE*/
			Spacket.OP = 1;
			Spacket.HTYPE = 0x01;
			Spacket.HLEN = 6;
			Spacket.HOPS = 0;
			Spacket.TID = 0;
			Spacket.SEC = 0;
			Spacket.FLAGS = 0;
			Spacket.CIP = cIP;
			Spacket.YIP = 0;
			Spacket.SIP = 0;
			Spacket.RIP = 0;
			memcpy(&Spacket.CHADDR,&curmacstr,sizeof(curmacstr));
			for(k=0;k<10;k++){
				Spacket.CHADDR[6+k]= 0x00;
			}
			for(k=0;k<64;k++){
				Spacket.SHNAME[k]=0x00;
			}
			for(k=0;k<128;k++){
				Spacket.FNAME[k]=0x00;
			}
			Spacket.cookie =0x63538263;
			Spacket.Options[0] = 0x35;
			Spacket.Options[1] = 0x01;
			Spacket.Options[2] = 0x07;
			Spacket.RestOP[0] = 0xff;
			break;
		case '4':		/*DHCPINFORM*/
			Spacket.OP = 1;
			Spacket.HTYPE = 0x01;
			Spacket.HLEN = 6;
			Spacket.HOPS = 0;
			Spacket.TID = 0;
			Spacket.SEC = 0;
			Spacket.FLAGS = 0;
			Spacket.CIP =cIP;
			Spacket.YIP = 0;
			Spacket.SIP = 0;
			Spacket.RIP = 0;
			memcpy(&Spacket.CHADDR,&curmacstr,sizeof(curmacstr));
			for(k=0;k<10;k++){
				Spacket.CHADDR[6+k]= 0x00;
			}
			for(k=0;k<64;k++){
				Spacket.SHNAME[k]=0x00;
			}
			for(k=0;k<128;k++){
				Spacket.FNAME[k]=0x00;
			}
			Spacket.cookie =0x63538263;
			Spacket.Options[0] = 0x35;
			Spacket.Options[1] = 0x01;
			Spacket.Options[2] = 0x08;
			Spacket.RestOP[0] = 0xff;
			break;			
	}
}

void SentPacket(){
	/*sent packet to server*/
	if ((sendto(sock, &Spacket,sizeof(Spacket) , 0,(struct sockaddr *) &ServAddr, sizeof(ServAddr)))!= sizeof(Spacket))
		printf("sendto() sent a different number of bytes than expected.\n");
}

void Expire(){
	for(;;){
		end =time(NULL);  
		if((lease*0.5) < difftime(end,start)){
			printf("T1 expire\n");
			break;
		}
	}
}

void RecvPacket(){
	    fromSize = sizeof(fromAddr);
        if ((recvMsgSize = recvfrom(sock,&Rpacket,sizeof(Rpacket),0,(struct sockaddr *) &fromAddr, &fromSize)) < 0)
			printf("recvfrom() failed.\n");
	else{
		switch (Rpacket.Options[2]) {  
			case 2: 
			printf("receive DHCPOFFER.\n");
			break;
			case 5: 
			printf("receive DHCPACK.\n");         
			memcpy(&IP, &Rpacket.CIP, sizeof(uint32_t));
			struct sockaddr_in sin1;
          		sin1.sin_addr.s_addr = IP;
           		IPPointer = inet_ntoa(sin1.sin_addr);
			SetIP("eth1", IPPointer);
			printf("IP: %s\n", IPPointer);
			Rpacket.RestOP[8] =Rpacket.RestOP[11];
			Rpacket.RestOP[11]=0x00;
			memcpy(&lease,&Rpacket.RestOP[8],sizeof(lease)); 
			break;
			case 6: 
			printf("receive DHCPNAK.\n");
			break;
		}
	}
}



int SetIP(char *ifname, char *Ipaddr){  
	int fd; 
	int rc;
	struct ifreq ifr; 
    struct sockaddr_in *sin;
    //struct rtentry rt;    
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(fd < 0){
        perror("socket   error");         
		return -1;     
   }
	memset(&ifr,0,sizeof(ifr)); 
    strcpy(ifr.ifr_name,ifname); 
	sin = (struct sockaddr_in*)&ifr.ifr_addr;     
	sin->sin_family = AF_INET;     
    // IP Address
	if(inet_aton(Ipaddr,&(sin->sin_addr)) < 0){     
        perror("inet_aton   error");     
		return -2;     
	}        
	if(ioctl(fd,SIOCSIFADDR,&ifr) < 0)   {     
        perror("ioctl   SIOCSIFADDR   error");     
        return -3;     
	}
	close(fd);   
	return rc;
}

int main(int argc, char *argv[]){
	int i = 1;
	struct sockaddr_in sin;
	struct ifreq if_eth1;
	strcpy(if_eth1.ifr_name, "eth1");
	socklen_t len = sizeof(i);
    comment = *argv[1];	
	/* Create a datagram/UDP socket */	
	if ((sock = socket(PF_INET, SOCK_DGRAM,IPPROTO_UDP)) < 0){
		printf("socket() failed.\n"); 
		exit(1);}
	/* allow socket to broadcast*/
	if(setsockopt(sock,SOL_SOCKET,SO_BROADCAST,&i,len)<0){
		printf("Error in set socket broadcast\n");
		exit(1);
	}
	/*set socket to interface eth1*/
	if (setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE,(char *)&if_eth1, sizeof(if_eth1))<0) {
		printf("bind socket to eth1 error\n");
		exit(1);}
	memset(&ClntAddr, 0, sizeof(ClntAddr));
	ClntAddr.sin_family = AF_INET;
	ClntAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	ClntAddr.sin_port =htons(ClientPort);
	if ((bind(sock, (struct sockaddr *) &ClntAddr,sizeof(ClntAddr))) < 0){
        printf("bind() failed.\n");
		exit(1);}
	memset(&ServAddr, 0, sizeof(ServAddr));
    ServAddr.sin_family = AF_INET;
	ServAddr.sin_addr.s_addr = inet_addr(broadcastIP);/*Server IP address 255.255.255.255 */
	ServAddr.sin_port = htons(ServPort); /* Server port */
	/*generate packet and sent packet*/
	switch(comment){
		case '1':	/*address acquisition*/
		memset(&Spacket, 0, sizeof(Spacket));
		SetPacket('1'); /*generate DHCPDISCOVER packet*/
		SentPacket(); /*sent packet*/	
		RecvPacket();
		memset(&Spacket, 0, sizeof(Spacket));
		SetPacket('2'); /*generate DHCPREQUEST packet*/
		SentPacket(); /*sent packet*/
		RecvPacket();
		break;
		case '2':	/*successful lease renew*/
		memset(&Spacket, 0, sizeof(Spacket));
		SetPacket('1'); /*generate DHCPDISCOVER packet*/
		SentPacket(); /*sent packet*/	
		RecvPacket();
		memset(&Spacket, 0, sizeof(Spacket));
		SetPacket('2'); /*generate DHCPREQUEST packet*/
		SentPacket(); /*sent packet*/
		RecvPacket();
		start = time(NULL);
		Expire();
		SetPacket('2'); /*generate DHCPREQUEST packet*/
		SentPacket(); /*sent packet*/
		RecvPacket();
		start = time(NULL);
		printf("Reset T1 T2.\n");
		/*reset t1 t2*/
		break;
		case '3':	/*failed lease renew (with an abnormal IP address)+address acquisition again*/
		abIP = argv[2];
		SetPacket('2'); /*generate DHCPREQUEST packet */
		memset(&ServAddr, 0, sizeof(ServAddr));
		ServAddr.sin_family = AF_INET;
		ServAddr.sin_port = htons(ServPort); /* Server port */
		ServAddr.sin_addr.s_addr = inet_addr(broadcastIP);
		SentPacket(); /*sent packet*/
		RecvPacket();
		if(Rpacket.Options[2] == 6){
		SetPacket('1'); /*generate DHCPDISCOVER packet*/
		SentPacket(); /*sent packet*/
		RecvPacket();
		comment = '0';
		SetPacket('2'); /*generate DHCPREQUEST packet*/
		SentPacket(); /*sent packet*/
		RecvPacket();}
		break;
		case '4':	/*failed lease renew (server down)*/
		memset(&Spacket, 0, sizeof(Spacket));
		SetPacket('1'); /*generate DHCPDISCOVER packet*/
		SentPacket(); /*sent packet*/	
		RecvPacket();
		memset(&Spacket, 0, sizeof(Spacket));
		SetPacket('2'); /*generate DHCPREQUEST packet*/
		SentPacket(); /*sent packet*/
		RecvPacket();
		start = time(NULL);
		Expire();
		
		memset(&Spacket, 0, sizeof(Spacket));
		SetPacket('2'); /*generate DHCPDISCOVER packet*/
		SentPacket();
		
	/*	if ((sock1 = socket(PF_INET, SOCK_DGRAM,IPPROTO_UDP)) < 0){
			printf("socket() failed.\n"); 
			exit(1);}
		if (setsockopt(sock1, SOL_SOCKET, SO_BINDTODEVICE,(char *)&if_eth1, sizeof(if_eth1))<0) {
		printf("bind socket to eth1 error\n");
		exit(1);}		
	/*	memset(&ClntAddr, 0, sizeof(ClntAddr));
		ClntAddr.sin_family = AF_INET;
		ClntAddr.sin_addr.s_addr = Rpacket.CIP;
		ClntAddr.sin_port =htons(ClientPort);
		memset(&ServAddr, 0, sizeof(ServAddr));
		ServAddr.sin_family = AF_INET;
		ServAddr.sin_addr.s_addr = Rpacket.SIP;/*Server IP address*/
	/*	ServAddr.sin_port = htons(ServPort); /* Server port */
	/*	SetPacket('2'); /*generate DHCPREQUEST packet*/
	/*	ServAddr.sin_port = htons(ServPort); /* Server port */
	/*if ((sendto(sock1, &Spacket,sizeof(Spacket) , 0,(struct sockaddr *) &ServAddr, sizeof(ServAddr)))!= sizeof(Spacket))
			printf("sendto() sent a different number of bytes than expected.\n");*/
		
		
		/*if ((bind(sock1, (struct sockaddr *) &ClntAddr,sizeof(ClntAddr))) < 0){
        printf("bind() failed.\n");
		exit(1);}/*
		/*sent packet*/	
		/*if ((sendto(sock1, &Spacket,sizeof(Spacket) , 0,(struct sockaddr *) &ServAddr, sizeof(ServAddr)))!= sizeof(Spacket))
			printf("sendto() sent a different number of bytes than expected.\n");*/
		/*sent packet*/	
		struct timeval timeout;
		timeout.tv_sec = lease*0.375;
		timeout.tv_usec = 0;    
		setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
		RecvPacket();	
		if(recvMsgSize == -1){
			printf("T2 expire.\n");
			recvMsgSize = 0;
		} 
		/*T2 expire, broadcast*/
		ServAddr.sin_addr.s_addr = inet_addr(broadcastIP);
		SetPacket('2'); /*generate DHCPREQUEST packet youwenti*/
		SentPacket(); /*sent packet*/
		timeout.tv_sec = lease*0.125;
		timeout.tv_usec = 0;    
		setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
		RecvPacket();	/*jiashijian*/
		if(recvMsgSize == -1){
			printf("Lease expire.\n");
			recvMsgSize = 0;
		} 
		/*lease expire*/
		SetPacket('1'); /*generate DHCPDISCOVER packet*/
		SentPacket(); /*sent packet*/
		RecvPacket();
		break;
		case '5':	/*inform*/
		SetPacket('4'); /*generate DHCPINFORM packet*/
		SentPacket(); /*sent packet*/
		RecvPacket();
		break;
		case '6': 		/*DHCPRELEASE*/
		SetPacket('3'); /*generate DHCPRELEASE packet*/
		SentPacket(); /*sent packet*/
		memcpy(&IP, &ClntAddr.sin_addr.s_addr, sizeof(uint32_t));
	        sin.sin_addr.s_addr = IP;
       		IPPointer = inet_ntoa(sin.sin_addr);
		SetIP("eth1", IPPointer);
		printf("IP: %s\n", IPPointer);
		break;
		case '7':		 /*release ip*/         
		memcpy(&IP, &ClntAddr.sin_addr.s_addr, sizeof(uint32_t));
	        sin.sin_addr.s_addr = IP;
       		IPPointer = inet_ntoa(sin.sin_addr);
		SetIP("eth1", IPPointer);
		printf("IP: %s\n", IPPointer);
		break;
	}
	close(sock);
}
