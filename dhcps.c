#include <stdio.h> /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), sendto()
and recvfrom() */
#include <arpa/inet.h> /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h> /* for atoi() and exit() */
#include <string.h> /* for memset() */
#include <unistd.h> /* for close() */
#include <net/if.h> /*for ifreq*/
#include <sys/socket.h>/*for setsockopt()*/
#include <fcntl.h>
#include <sys/ioctl.h>


struct DHCPpacket Rpacket;
int sock, i = 1; /* Socket */
struct sockaddr_in ServAddr; /* Local address */
struct sockaddr_in ClntAddr; /* Client address */
unsigned int CliAddrLen; /* Length of client address */
unsigned short ServPort = 67; /* Server port */
unsigned short ClientPort = 68;
int recvMsgSize; /* Size of received message */
struct ifreq if_eth1;
char broadcastIP[] = "255.255.255.255";
char ClientIP[12];
char Gateway[12];
char DNS[12];
int lease;

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
	uint8_t RestOP[30];
} DHCP; 

void Release(){  
		char buf[12],buf1[12];
		FILE *file;
        FILE *file1;
        if((file = fopen("dhcp.lease","r"))==NULL)
                printf("Error in opening\n");
		if((file1 = fopen("dhcp.rest","w"))==NULL)
                printf("Error in opening\n");
		while (fscanf(file,"%s",buf)!= EOF){
		if(Rpacket.CIP != inet_addr(buf)){
			fprintf(file1,"%s\n",buf);}
		}
		fclose(file);
		fclose(file1);
		remove("dhcp.lease");
		rename("dhcp.rest","dhcp.lease");
}

int Compare(){
	char buf[12];
	FILE *file;
	int test = 0;
	if((file = fopen("dhcp.lease","r"))==NULL)
        printf("Error in opening\n");
	while (fscanf(file,"%s",buf)!= EOF){
		if(Rpacket.CIP ==inet_addr(buf)){
			test = 1;
			break;
		}
	}
	fclose(file);
	return test;
}

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

void GetIP(){ 
    char buf[12];
	char buf1[12];
	memset(&buf, 0, sizeof(buf));
	memset(&buf1, 0, sizeof(buf1));
	FILE *file;
	FILE *file1;
	int i = 0;
    int test = 0,test1;
        if((file = fopen("dhcp.config","r"))==NULL)
                printf("Error in opening\n");		
		while (fscanf(file,"%s",buf)!= EOF){
			test1 = 0;
			if(i == 0)
			memcpy(&DNS,&buf,sizeof(buf));		
			else if(i ==1)
			memcpy(&Gateway,&buf,sizeof(buf));
			else{
			if((file1 = fopen("dhcp.lease","r"))==NULL)
                printf("Error in opening\n");
			while (fscanf(file1,"%s",buf1)!= EOF){
				test = 1;
				if(strcmp(buf, buf1) == 0){
					fclose(file1);
					test1 = 1;
                   	break;}	
				memset(&buf1, 0, sizeof(buf1));			
			}
			if((test == 0) || (test1 == 0)){
				fclose(file1);
				if((file1 = fopen("dhcp.lease","a"))==NULL)
                printf("Error in opening\n");
				fprintf(file1,"%s\n",buf);
				fclose(file1);
				break;
			}
			memset(&buf, 0, sizeof(buf));
			}
		i++;
		}
		fclose(file);
    		 memcpy(&ClientIP,&buf,sizeof(buf));
		
}   

void SetPacket(char type){
	unsigned int IP=getIP();
   
	uint32_t DIP;
	uint32_t GIP; 
	uint32_t mask = inet_addr("255.255.255.0");
	Rpacket.RestOP[0] = 0x06;   /*dns*/
	Rpacket.RestOP[1] = 0x04;
	Rpacket.RestOP[6] = 0x33;	/*lease*/
	Rpacket.RestOP[7] = 0x04;
	Rpacket.RestOP[8] = lease;
	Rpacket.RestOP[11] = Rpacket.RestOP[8] ;
	Rpacket.RestOP[8] = 0x00;
	Rpacket.RestOP[13] = 0x01;	/*mask*/
	Rpacket.RestOP[14] = 0x04;	
	memcpy(&Rpacket.RestOP[15],&mask,sizeof(mask));
	Rpacket.RestOP[19] = 0x03;	/*gateway*/
	Rpacket.RestOP[20] = 0x04;	
	Rpacket.RestOP[25] = 0xff;
	switch (type){
		case '1':    /*DHCPOFFER*/
		Rpacket.SIP =IP;
		Rpacket.FLAGS = 0x0080;
		Rpacket.OP = 2;
		Rpacket.Options[0] = 0x35;
		Rpacket.Options[1] = 0x01;
		Rpacket.Options[2] = 0x02;
		break;
		case '2':    /*DHCPACK*/

		Rpacket.SIP =IP ;
		Rpacket.OP = 2;
		Rpacket.Options[0] = 0x35;
		Rpacket.Options[1] = 0x01;
		Rpacket.Options[2] = 0x05;
		if(Rpacket.CIP == 0){
		GetIP();
		DIP = inet_addr(DNS);
		GIP = inet_addr(Gateway);
		memcpy(&Rpacket.RestOP[2],&DIP,sizeof(DIP));
		memcpy(&Rpacket.RestOP[21],&GIP,sizeof(GIP));
		Rpacket.CIP = inet_addr(ClientIP);}
		break;
		case '3':    /*DHCPNAK*/
		Rpacket.SIP =IP ;
		Rpacket.OP = 2;
		Rpacket.Options[0] = 0x35;
		Rpacket.Options[1] = 0x01;
		Rpacket.Options[2] = 0x06;
		break;
	}
}

void SentPacket(){
	/*sent packet to client*/
	if ((sendto(sock, &Rpacket,sizeof(Rpacket),0,(struct sockaddr *) &ClntAddr, sizeof(ClntAddr)))!= sizeof(Rpacket)){
        printf("sendto() sent a different number of bytes than expected.\n");
	}
}

int RecvPacket(){
		CliAddrLen = sizeof(ClntAddr);
        memset(&Rpacket, 0, sizeof(Rpacket));
		if ((recvMsgSize = recvfrom(sock,&Rpacket,sizeof(Rpacket),0,(struct sockaddr *) &ClntAddr, &CliAddrLen)) < 0){
			printf("recvfrom() failed.\n");
			return 0;}
		else 
			return 1;
}

void Reply(){
	for (;;) /* Run forever */{
		if(RecvPacket()){
			switch (Rpacket.Options[2]) {  
				case 1:  /*receive DHCP_DISCOVER*/
					memset(&ClntAddr, 0, sizeof(ClntAddr));
					ClntAddr.sin_family = AF_INET;
					ClntAddr.sin_addr.s_addr = inet_addr(broadcastIP);/* client IP address 255.255.255.255 */
					ClntAddr.sin_port = htons(ClientPort); /* client port */
					printf("receive DHCP_DISCOVER.\n");
					SetPacket('1');
					SentPacket();
					break;   
				case 7:  /*receive DHCPRELEASE*/
					printf("receive DHCPRELEASE.\n");
					Release();   
					break;  
				case 3:	/*receive DHCPREQUEST*/
					printf("receive DHCPREQUEST.\n"); 
					memset(&ClntAddr, 0, sizeof(ClntAddr));
					ClntAddr.sin_family = AF_INET;
					ClntAddr.sin_addr.s_addr = inet_addr(broadcastIP);/* client IP address 255.255.255.255 */
					ClntAddr.sin_port = htons(ClientPort); /* client port */
					if(Rpacket.CIP == 0 || Compare()){
						SetPacket('2');
						SentPacket();}
					else{
						printf("Receive an abnormal IP address!\n");
						SetPacket('3');
						SentPacket();
					}
					break;
				case 8:	/*receive DHCPINFORM*/
					printf("receive DHCPINFORM.\n"); 
					ClntAddr.sin_family = AF_INET;
			
					ClntAddr.sin_addr.s_addr = inet_addr(broadcastIP);/* client IP address 255.255.255.255*/
					ClntAddr.sin_port = htons(ClientPort); /* client port */
					SetPacket('2');
					SentPacket();
					break;
			} 
		}
	}
}

int main(int argc, char *argv[]){
	char t[4];
	memcpy(&t,argv[1], sizeof(t));
	lease = atoi(t);
	printf("%d\n",lease);
	strcpy(if_eth1.ifr_name, "eth1");
	socklen_t len = sizeof(i);
	/* Create socket for sending/receiving datagrams */
	if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
	printf("socket() failed.\n");
	/* allow socket to broadcast*/
	if(setsockopt(sock,SOL_SOCKET,SO_BROADCAST,&i,len)<0){
		printf("Error in set socket broadcast\n");}
	/*set socket to interface eth1*/
	if (setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE,(char *)&if_eth1, sizeof(if_eth1))<0) {
		printf("bind socket to eth1 error\n");}
	/* Construct server structure */
	memset(&ServAddr, 0, sizeof(ServAddr));
	ServAddr.sin_family = AF_INET;
	ServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	ServAddr.sin_port =htons(ServPort);
	/* Bind to the server address port 67*/
	if ((bind(sock, (struct sockaddr *) &ServAddr,sizeof(ServAddr))) < 0)
		printf("bind() failed.\n");
	Reply();

}
