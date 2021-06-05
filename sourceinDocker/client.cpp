#define PORT 7208
#define MAXLINE 1024
static __inline__ unsigned long long rdtsc(void)
{
	unsigned long long int x;
	__asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
	// broken for 64-bit builds; don't copy this code
	return x;
}

int main(){

	int sockfd;
	char buffer[MAXLINE];
	char *hello = "hello from client";
	struct sockaddr_in servaddr;

	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) <0){
		return -1;
	}

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = inet_addr("10.255.99.3");
	int n;
	socklen_t len;
	printf("Start:\n");
	uint64_t acc = 0;
	uint64_t start = 0;
	for(int i=0;i<1000000;i++){
		start = rdtsc();
		sendto(sockfd, (const char*)hello, strlen(hello), MSG_CONFIRM, (const struct sockaddr*)&servaddr, sizeof(servaddr));
		n = recvfrom(sockfd, (char*)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr*) &servaddr, &len);
		uint64_t calc = rdtsc() - start;
		if(i>500000 && i%50000 == 0){
			printf("%d %llu\n",i,calc);
		}
	}

}

