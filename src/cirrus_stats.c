#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#define __USE_XOPEN
#include <time.h>
#include "mysql.h"
#include "mysql.c"
#include "commun.c"
#include "password.h"

struct Compte
{
char nom[100];
char quota[100];
};

int main()
{
char *envoi;
int i,start,succes,m,j,n;
char *query,*res,*reponse,*lienretour,*textebouton,*totpsecret,*totpcode;
char *mdp,*code;
struct Compte *compte;

envoi=read_POST();
start=strlen(envoi);
for(i=0;i<start;i++)
	{
	if(envoi[i]<32) envoi[i]=0;
	}
query_size=atoi(getenv("CONTENT_LENGTH"));
compte=(struct Compte *)malloc(10000*sizeof(struct Compte));
mdp=(char*)malloc(query_size);
code=(char*)malloc(query_size);
res=(char*)malloc(100);
reponse=(char*)malloc(200);
lienretour=(char*)malloc(100);
textebouton=(char*)malloc(20);
totpsecret=(char*)malloc(40);
totpcode=(char*)malloc(40);
query=(char*)malloc(20000+query_size);
get_chaine(envoi,2,mdp);
tamb(mdp);
get_chaine(envoi,1,code);
tamb(code);
succes=0;
if((handler=db_opendatabase("cirrus","localhost","cirrus",PASSWORD))==NULL)
	{
	strcpy(reponse,"Erreur : Impossible d'accéder à la base de données");
	strcpy(lienretour,"/cirrus/index.html");
	strcpy(textebouton,"Réessayer");
	}
else
	{
	sprintf(query,"select * from compte where nom=\'Admin\' and mdp=\'%s\';",mdp);
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
		strcpy(query,"select totpsecret,totpcode from compte where nom=\'Admin\';");
      db_query(handler,query);
      db_getvalue(result,0,0,totpsecret,40);
      db_getvalue(result,0,1,totpcode,40);
      db_clear_result(result);
      if(strcmp(totpsecret,"")==0) succes=1;
      else
      	{
      	if(strcmp(totpcode,code)==0) succes=1;
      	else
      		{
      		strcpy(reponse,"Erreur : Code de vérification erroné ...");
      		strcpy(lienretour,"/cirrus/index.html");
            strcpy(textebouton,"Réessayer");
            sleep(2);
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
<link rel=\"icon\" type=\"image/x-icon\" href=\"/cirrus/cirrus2.png\">\n\
<link rel=\"stylesheet\" href=\"/cirrus/themes/ohm.min.css\" />\n\
<link rel=\"stylesheet\" href=\"/cirrus/themes/jquery.mobile.icons.min.css\" />\n\
<link rel=\"stylesheet\" href=\"/jq/jquery.mobile.structure-1.4.5.min.css\" />\n\
<script src=\"/jq/demos/js/jquery.min.js\"></script>\n\
<script src=\"/jq/jquery.mobile-1.4.5.min.js\"></script>\n\
</head>\n\
<body>\n\
   <div data-role=\"page\" data-theme=\"b\">\n\
      <div data-role=\"header\" data-position=\"fixed\" data-theme=\"b\">\n\
      <div data-role=\"navbar\">\n\
            <ul>\n\
               <li>\n\
                  <form action=\"/cgi-bin/cirrus_folders.cgi\" method=\"post\" id=\"form-refresh\" data-transition=\"none\" data-rel=\"dialog\">\n\
                  <input type=\"hidden\" name=\"txt-nom\" id=\"txt-nom\" value=\"Admin\">\n\
                  <input type=\"hidden\" name=\"txt-password\" id=\"txt-password\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-code\" id=\"txt-code\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-enter\" id=\"txt-enter\" value=\"n\">\n\
                  <button type=\"submit\" data-mini=\"false\" data-inline=\"false\" data-theme=\"c\" data-icon=\"back\" class=\"ui-shadow\">Retour</button>\n\
                  </form>\n\
               </li>\n\
            </ul>\n\
         </div>\n\
         <h1><img src=\"/cirrus/cirrus.png\"></h1>\n\
      </div>\n\
      <div role=\"main\" class=\"ui-content\">\n",mdp,code);
if(succes==1)
	{
	sprintf(query,"select nom,quota from compte where nom!=\'Admin\';");
	db_query(handler,query);
	n=db_ntuples(result);
	for(i=0;i<n;i++)
		{
		db_getvalue(result,i,0,compte[i].nom,100);
		db_getvalue(result,i,1,compte[i].quota,100);
		}
	db_clear_result(result);
	printf("<ul data-role=\"listview\" data-inset=\"true\" data-divider-theme=\"b\">\n");
	for(i=0;i<n;i++)
		{
		sprintf(query,"select taille from entree where owner=\'%s\';",compte[i].nom);
		db_query(handler,query);
		m=db_ntuples(result);
		start=0;
		for(j=0;j<m;j++)
			{
			db_getvalue(result,j,0,reponse,200);
			start += atoi(reponse);
			}
		start=start/1024;
		db_clear_result(result);
		printf("<li data-role=\"list-divider\">%s - %d Go sur %s Go</li>\n",compte[i].nom,start,compte[i].quota);
		sprintf(query,"select taille from entree where zone=\"musique\" and owner=\'%s\';",compte[i].nom);
		db_query(handler,query);
		m=db_ntuples(result);
		start=0;
		for(j=0;j<m;j++)
			{
			db_getvalue(result,j,0,reponse,200);
			start += atoi(reponse);
			}
		start=start/1024;
		printf("<li><img src=\"/cirrus/music.png\"> %d fichiers - %d Go</li>\n",m,start);
		db_clear_result(result);
		sprintf(query,"select taille from entree where zone=\"image\" and owner=\'%s\';",compte[i].nom);
		db_query(handler,query);
		m=db_ntuples(result);
		start=0;
		for(j=0;j<m;j++)
			{
			db_getvalue(result,j,0,reponse,200);
			start += atoi(reponse);
			}
		start=start/1024;
		printf("<li><img src=\"/cirrus/image.png\"> %d fichiers - %d Go</li>\n",m,start);
		db_clear_result(result);
		sprintf(query,"select taille from entree where zone=\"video\" and owner=\'%s\';",compte[i].nom);
		db_query(handler,query);
		m=db_ntuples(result);
		start=0;
		for(j=0;j<m;j++)
			{
			db_getvalue(result,j,0,reponse,200);
			start += atoi(reponse);
			}
		start=start/1024;
		printf("<li><img src=\"/cirrus/video.png\"> %d fichiers - %d Go</li>\n",m,start);
		db_clear_result(result);
		sprintf(query,"select taille from entree where zone=\"document\" and owner=\'%s\';",compte[i].nom);
		db_query(handler,query);
		m=db_ntuples(result);
		start=0;
		for(j=0;j<m;j++)
			{
			db_getvalue(result,j,0,reponse,200);
			start += atoi(reponse);
			}
		start=start/1024;
		printf("<li><img src=\"/cirrus/document.png\"> %d fichiers - %d Go</li>\n",m,start);
		db_clear_result(result);
		sprintf(query,"select taille from entree where zone=\"pageweb\" and owner=\'%s\';",compte[i].nom);
		db_query(handler,query);
		m=db_ntuples(result);
		start=0;
		for(j=0;j<m;j++)
			{
			db_getvalue(result,j,0,reponse,200);
			start += atoi(reponse);
			}
		start=start/1024;
		printf("<li><img src=\"/cirrus/pageweb.png\"> %d fichiers - %d Go</li>\n",m,start);
		db_clear_result(result);
		}
	db_close(handler);
	}
else
	{
	printf("%s<br>\n\
<a href=\"%s\" data-role=\"button\" data-icon=\"back\" data-theme=\"c\">%s</a><br><br>\n",reponse,lienretour,textebouton);
	}
printf("</div>\n\
      <div data-role=\"footer\" class=\"ui-bar\" data-position=\"fixed\" data-theme=\"b\">\n\
         <h4>Cirrus - Cloud privé</h4>\n\
      </div>\n\
   </div>\n\
</body>\n\
</html>\n");
}
