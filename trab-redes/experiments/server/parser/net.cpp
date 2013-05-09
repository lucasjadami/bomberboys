#include <cstdio>
#include <cstring>

int main()
{
	char line[1000];
	double totalIn, totalOut;
	int inCount = 0;
	int outCount = 0;
	
	while (gets(line) != NULL)
	{
		char firstWord[100];
		sscanf(line, "%s", firstWord);
		
		if (strcmp(firstWord, "IpExtInOctets") == 0)
		{
			long long packets;
			sscanf(line, " %*s %lld ", &packets);
			inCount++;
			totalIn += packets;
		}
		else if (strcmp(firstWord, "IpExtOutOctets") == 0)
		{
			long long packets;
			sscanf(line, " %*s %lld ", &packets);
			outCount++;
			totalOut += packets;
		}
	}
	
	printf("%lf %lf\n", totalIn / inCount, totalOut / outCount);
	
	return 0;
}
