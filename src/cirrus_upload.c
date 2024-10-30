#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#define __USE_XOPEN
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "mysql.h"
#include "mysql.c"
#include "commun.c"
#include "password.h"
#include "specific.h"

int main()
{
char *envoi;
int i,start,success,fd,fd1,nb,len,amp,quotatteint,quotacible,id;
char *query,a,*reponse,*lienretour,*textebouton,*totpsecret,*totpcode,*buffer,*nomfic,*chemin;
char *nom,*mdp,*code,*quota,*mode,*storage_dir,*Id;
struct stat statbuf;

//printf("Content-Type: text/html; charset=utf-8\r\n\r\n<html><body>\n");
char *content_type = getenv("CONTENT_TYPE");
//printf("content type: %s<br>", content_type);
i=strlen(content_type)-1;
while(content_type[i]!='-') i--;
amp=strlen(content_type)-(i-5)+2;
query_size = atoi(getenv("CONTENT_LENGTH"));
//printf("content length %d<br>", query_size);
success=0;
nom=(char*)malloc(1000+query_size);
mdp=(char*)malloc(1000+query_size);
code=(char*)malloc(1000+query_size);
mode=(char*)malloc(1000+query_size);
lienretour=(char*)malloc(1000);
textebouton=(char*)malloc(1000);
totpsecret=(char*)malloc(40);
totpcode=(char*)malloc(40);
quota=(char*)malloc(40);
Id=(char*)malloc(40);
buffer=(char*)malloc(1000);
nomfic=(char*)malloc(1000);
chemin=(char*)malloc(1000);
storage_dir=(char*)malloc(256);
strcpy(storage_dir,STORAGE_DIR);
reponse=(char*)malloc(20000);
query=(char*)malloc(5000+query_size);
nb=read(0,buffer,1000);
buffer[nb]=0;
i=0;
while(buffer[i]!=0x0D || buffer[i+1]!=0x0A) i++;
i+=2;
while(buffer[i]!=0x0D || buffer[i+1]!=0x0A) i++;
i+=2;
while(buffer[i]!=0x0D || buffer[i+1]!=0x0A) i++;
i+=2;
start=i;
while(buffer[i]!=0x0D)
	{
	if(buffer[i]=='\'') buffer[i]=' ';
	nom[i-start]=buffer[i];
	i++;
	}
nom[i-start]=0;
//printf("Nom %s<br>",nom);
i+=2;
while(buffer[i]!=0x0D || buffer[i+1]!=0x0A) i++;
i+=2;
while(buffer[i]!=0x0D || buffer[i+1]!=0x0A) i++;
i+=2;
while(buffer[i]!=0x0D || buffer[i+1]!=0x0A) i++;
i+=2;
start=i;
while(buffer[i]!=0x0D)
	{
	mdp[i-start]=buffer[i];
	i++;
	}
mdp[i-start]=0;
//printf("Mdp %s<br>",mdp);
i+=2;
while(buffer[i]!=0x0D || buffer[i+1]!=0x0A) i++;
i+=2;
while(buffer[i]!=0x0D || buffer[i+1]!=0x0A) i++;
i+=2;
while(buffer[i]!=0x0D || buffer[i+1]!=0x0A) i++;
i+=2;
start=i;
while(buffer[i]!=0x0D)
	{
	code[i-start]=buffer[i];
	i++;
	}
code[i-start]=0;
//printf("Code %s<br>",code);
i+=2;
while(buffer[i]!=0x0D || buffer[i+1]!=0x0A) i++;
i+=2;
while(buffer[i]!=0x0D || buffer[i+1]!=0x0A) i++;
i+=2;
while(buffer[i]!=0x0D || buffer[i+1]!=0x0A) i++;
i+=2;
start=i;
while(buffer[i]!=0x0D)
	{
	mode[i-start]=buffer[i];
	i++;
	}
mode[i-start]=0;
//printf("Mode %s<br>",mode);
i+=2;
while(buffer[i]!='f' || buffer[i+1]!='i' || buffer[i+2]!='l' || buffer[i+3]!='e' || buffer[i+4]!='n' || buffer[i+5]!='a' || buffer[i+6]!='m' || buffer[i+7]!='e') i++;
i+=10;
start=i;
while(buffer[i]!='"')
	{
	if(buffer[i]==0x27) buffer[i]=' ';
	nomfic[i-start]=buffer[i];
	i++;
	}
nomfic[i-start]=0;
//printf("Nom fichier %s<br>",nomfic);
while(buffer[i]!=0x0D || buffer[i+1]!=0x0A) i++;
i+=2;
while(buffer[i]!=0x0D || buffer[i+1]!=0x0A) i++;
i+=2;
while(buffer[i]!=0x0D || buffer[i+1]!=0x0A) i++;
i+=2;
start=i;
success=0;
if((handler=db_opendatabase("cirrus","localhost","cirrus",PASSWORD))==NULL)
	{
	strcpy(reponse,"Erreur : Impossible d'accéder à la base de données");
	strcpy(lienretour,"/cirrus/index.html");
	strcpy(textebouton,"Réessayer");
	}
else
	{
	sprintf(query,"select * from compte where nom=\'%s\';",nom);
	db_query(handler,query);
	if(db_ntuples(result)==0)
		{
		strcpy(reponse,"Erreur : Ce compte n'existe pas ...");
		strcpy(lienretour,"/cirrus/index.html");
		strcpy(textebouton,"Réessayer");
		db_clear_result(result);
		db_close(handler);
		}
	else
		{
		db_clear_result(result);
		sprintf(query,"select * from compte where nom=\'%s\' and mdp=\'%s\';",nom,mdp);
		db_query(handler,query);
		if(db_ntuples(result)==0)
			{
			strcpy(reponse,"Erreur : Mauvais mot de passe ...");
			strcpy(lienretour,"/cirrus/index.html");
			strcpy(textebouton,"Réessayer");
			db_clear_result(result);
			db_close(handler);
			sleep(2);
			}
		else
			{
			db_clear_result(result);
			sprintf(query,"select * from compte where nom=\'%s\' and mdp=\'%s\' and actif=\'oui\';",nom,mdp);
			db_query(handler,query);
			if(db_ntuples(result)==0)
				{
				strcpy(reponse,"Erreur : Votre compte n'a pas été activé par l'administrateur ...");
				strcpy(lienretour,"/cirrus/index.html");
				strcpy(textebouton,"Réessayer");
				db_clear_result(result);
				db_close(handler);
				}
			else
				{
				db_clear_result(result);
				sprintf(query,"select totpsecret,totpcode from compte where nom=\'%s\';",nom);
      		db_query(handler,query);
      		db_getvalue(result,0,0,totpsecret,40);
      		db_getvalue(result,0,1,totpcode,40);
      		db_clear_result(result);
      		if(strcmp(totpsecret,"")==0) success=1;
      		else
         		{
         		if(strcmp(totpcode,code)==0) success=1;
         		else
            		{
            		strcpy(reponse,"Erreur : Code de vérification erroné ...");
            		strcpy(lienretour,"/cirrus/index.html");
            		strcpy(textebouton,"Réessayer");
            		sleep(2);
            		}
         		}
         	if(success==1)
         		{
         		sprintf(query,"select quota from compte where nom=\'%s\';",nom);
					db_query(handler,query);
					db_getvalue(result,0,0,quota,40);
					db_clear_result(result);
					sprintf(query,"select taille from entree where owner=\'%s\';",nom);
					db_query(handler,query);
					quotatteint=0;
					for(i=0;i<db_ntuples(result);i++)
						{
						db_getvalue(result,i,0,reponse,200);
						quotatteint += atoi(reponse);
						}
					quotacible=atoi(quota)*1024;
					db_clear_result(result);
         		success=0;
         		if(strcmp(mode,"musique")==0)
         			{
         			sprintf(reponse,"%s/musique",nom);
         			success=1;
         			}
         		if(strcmp(mode,"video")==0)
         			{
         			sprintf(reponse,"%s/video",nom);
         			success=1;
         			}
         		if(strcmp(mode,"document")==0)
         			{
         			sprintf(reponse,"%s/document",nom);
         			success=1;
         			}
         		if(strcmp(mode,"image")==0)
         			{
         			sprintf(reponse,"%s/image",nom);
         			success=1;
         			}
         		if(strcmp(mode,"pageweb")==0)
         			{
         			sprintf(reponse,"%s/pageweb",nom);
         			success=1;
         			}
         		if(success==0) strcpy(reponse,"Erreur : mauvais code de type de fichier");
         		else
         			{
						//printf("%s</body></html>",buffer);
         			i=start;
						sprintf(chemin,"%s/%s/%sa",storage_dir,reponse,nomfic);
						fd=open(chemin,O_CREAT|O_TRUNC|O_WRONLY,0644);
						write(fd,&(buffer[i]),nb-i);
						if(nb==1000)
							{
							while((nb=read(0,buffer,1000))>0) write(fd,buffer,nb);
							}
						close(fd);
						sprintf(chemin,"%s/%s/%sa",storage_dir,reponse,nomfic);
						fd=open(chemin,O_RDONLY);
						fstat(fd,&statbuf);
						if(quotatteint+(statbuf.st_size/(1024*1024))>quotacible)
							{
							success=0;
							strcpy(reponse,"Erreur : Upload Impossible, vous dépassez le quota alloué");
							}
						else
							{
							start=statbuf.st_size-amp-4;
							sprintf(chemin,"%s/%s/%s",storage_dir,reponse,nomfic);
							fd1=open(chemin,O_CREAT|O_TRUNC|O_WRONLY,0644);
							/*for(i=0;i<start;i++)
								{
								read(fd,&a,1);
								write(fd1,&a,1);
								}*/
							i=0;
							while(i<start)
								{
								nb=read(fd,buffer,1000);
								if(i+nb<start)
									{
									write(fd1,buffer,nb);
									i+=nb;
									}
								else
									{
									write(fd1,buffer,start-i);
									i=start;
									}
								}
							close(fd);
							close(fd1);
							sprintf(chemin,"%s/%s/%sa",storage_dir,reponse,nomfic);
							unlink(chemin);
							sprintf(buffer,"/usr/bin/openssl dgst -sha256 \'%s/%s/%s\' > \'%s/%s/dgst\'",storage_dir,reponse,nomfic, storage_dir,reponse);
							system(buffer);
							sprintf(chemin,"%s/%s/dgst",storage_dir,reponse);
							fd=open(chemin,O_RDONLY);
							nb=read(fd,buffer,1000);
							buffer[nb]=0;
							close(fd);
							unlink(chemin);
							i=0;
							while(buffer[i]!='=') i++;
							i+=2;
							start=i;
							while(buffer[i]!=0x0A)
								{
								buffer[i-start]=buffer[i];
								i++;
								}
							buffer[i-start]=0;
							if(strcmp(mode,"pageweb")==0) strcat(buffer,".html");
                            sprintf(query,"mv \'%s/%s/%s\' \'%s/%s/%s\'",storage_dir,reponse,nomfic,storage_dir,reponse,buffer);
							system(query);
							sprintf(query,"select id from entree where owner=\'%s\' order by id desc;",nom);
							db_query(handler,query);
							if(db_ntuples(result)>0) db_getvalue(result,0,0,Id,40);
							else strcpy(Id,"0");
							db_clear_result(result);
							id=atoi(Id)+1;
							sprintf(query,"insert into entree values (\'%s\',\'%s\',\'%s\',\'/cirrus/storage/%s/%s/%s\',%d,%d);",nom,mode,nomfic,nom,mode,buffer,(int)(statbuf.st_size/(1024*1024)),id);
							db_query(handler,query);
							db_clear_result(result);
							}
						}
					}
				//db_close(handler);
				}
			}
		}
	}
printf("Content-Type: text/html\n\n\
<!DOCTYPE html>\n\
<html>\n\
<head>\n\
<title>Cirrus</title>\n\
<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n\
<meta charset=\"utf-8\">\n\
</head>\n\
<body>\n");
if(success==1)
	{
	printf("Fichier transféré dans le cloud");
	}
else
	{
	printf("%s",reponse);
	}
printf("</body>\n\
</html>\n");
}