#define MAX_MESSAGES_PER_PAGE 250

#include <time.h>
//#include "specific.c"

int query_size;

int get_weekday(char * str) {
  struct tm tm;
  memset((void *) &tm, 0, sizeof(tm));
  if (strptime(str, "%Y-%m-%d", &tm) != NULL) {
    time_t t = mktime(&tm);
    if (t >= 0) {
      return localtime(&t)->tm_wday; // Sunday=0, Monday=1, etc.
    }
  }
  return -1;
}

void alerte(char *ch)
{
printf("%s\n",ch);
}

void elimine_blancs(char *chaine)
{
int i,start;
char *res;

res=(char *)malloc(strlen(chaine)+1);
i=start=0;
do
	{
	res[start]=chaine[i];
	if(res[start]!=' ') start++;
	i++;
	}
while(i<=strlen(chaine));
strcpy(chaine,res);
free(res);
}

void prepare_contenu(char *chaine)
{
int i,start;
char *res;

res=(char *)malloc(strlen(chaine)*2);
i=start=0;
do
	{
	if(chaine[i]=='\'')
		{
		res[start]='\\';
		start++;
		}
	res[start]=chaine[i];
	start++;
	i++;
	}
while(i<=strlen(chaine));
strcpy(chaine,res);
free(res);
}

void magiczero(int num, char *chaine)
{
if(num<10) sprintf(chaine,"0%d",num);
else sprintf(chaine,"%d",num);
}

void url_decode(char *chaine)
{
int i,start;
char *res,buf[50];

start=i=0;
res=(char *)malloc(10*strlen(chaine));
while(i<=strlen(chaine))
	{
	if(chaine[i]!='%') 
		{
		res[start]=chaine[i];
		i++;
		start++;
		}
	else
		{
		if(chaine[i+1]!='3' || chaine[i+2]!='C')
			{
			sprintf(buf,"0x%c%c",chaine[i+1],chaine[i+2]);
			res[start]=strtol(buf,NULL,16);
			start++;
			i = i+3;
			}
		else
			{
			res[start]='&';
			res[start+1]='l';
			res[start+2]='t';
			res[start+3]=';';
			start = start+4;
			i = i+3;
			}
		}
	}
strcpy(chaine,res);
free(res);
}

char* read_POST() 
{
query_size=atoi(getenv("CONTENT_LENGTH"));
char* query_string = (char*) malloc(query_size);
if (query_string != NULL) fread(query_string,query_size,1,stdin);
return query_string;
}

int max_getchaine(char *chaine)
{
int i,somme;

somme=0;
for(i=0;i<strlen(chaine);i++)
	{
	if(chaine[i]=='=') somme++;
	}
return(somme);
}

int get_chaine(char *chaine,int pos,char *resultat)
{
int i,n,start;

i=strlen(chaine);
for(n=0; n<pos;n++) 
	{
	while(chaine[i]!='=') i--;
	i--;
	}
i+=2;
start=i;
do
	{
	resultat[i-start]=chaine[i];
	i++;
	}
while(chaine[i-1]!=0 && chaine[i-1]!='&');
resultat[i-start-1]=0;
}

int get_chaine2(char *chaine,int pos,char *resultat)
{
int i,n,start;

i=strlen(chaine);
for(n=0; n<pos;n++) 
	{
	while(chaine[i]!='&' && i>=0) i--;
	i--;
	}
i+=2;
start=i;
do
	{
	resultat[i-start]=chaine[i];
	i++;
	}
while(chaine[i-1]!=0 && chaine[i-1]!='=');
resultat[i-start-1]=0;
}

void klog(char *ph)
{
int fd;
char a;

a='\n';
fd=open("/tmp/res",O_CREAT|O_WRONLY,0644);
write(fd,ph,strlen(ph));
write(fd,&a,1);
close(fd);
}

void tamb(char *avatar)
{
int i;

for(i=0;i<strlen(avatar);i++)
	{
	if(avatar[i]=='+') avatar[i]=' ';
	}
url_decode(avatar);
prepare_contenu(avatar);
}