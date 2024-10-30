#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#define __USE_XOPEN
#include <time.h>

int main()
{
char *envoi;
int i,j,start,success;
char *query,*reponse;
char *url;

query=(char *)malloc(strlen(getenv("QUERY_STRING"))+1000);
url=(char *)malloc(strlen(getenv("QUERY_STRING"))+10);
reponse=(char *)malloc(20000);
strcpy(query,getenv("QUERY_STRING"));
start=0;
i=strlen(query)-1;
while(query[i]!='=') i--;
i++;
while(i<=strlen(query))
	{
	url[start]=query[i];
	i++;
	start++;
	}
printf("Content-Type: text/html\n\n\
<!DOCTYPE html>\n\
<html>\n\
<body>\n\
<video controls src=\"%s\" style=\"max-width:100%%;\" controls>Votre navigateur ne supporte pas l'affichage des vidéos</video>\n\
</body>\n\
</html>\n",url);
}