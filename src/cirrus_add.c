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

int main()
{
char *envoi;
int i,start,success,nb_entrees;
char *query,*reponse,*lienretour,*textebouton;
char *nom,*mdp,*code,*mode;

envoi=read_POST();
success=0;
start=strlen(envoi);
for(i=0;i<start;i++)
	{
	if(envoi[i]<32) envoi[i]=0;
	}
query_size=atoi(getenv("CONTENT_LENGTH"));
nom=(char*)malloc(query_size);
mdp=(char*)malloc(query_size);
code=(char*)malloc(query_size);
mode=(char*)malloc(query_size);
lienretour=(char*)malloc(100);
textebouton=(char*)malloc(20);
reponse=(char*)malloc(200);
query=(char*)malloc(500+query_size);
get_chaine(envoi,4,nom);
tamb(nom);
get_chaine(envoi,3,mdp);
tamb(mdp);
get_chaine(envoi,2,code);
tamb(code);
get_chaine(envoi,1,mode);
tamb(mode);
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
				success=1;
				db_clear_result(result);
         	db_close(handler);
				}
			}
		}
	}
if(success!=1)
	{
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
      <div data-role=\"header\" data-position=\"fixed\" data-theme=\"b\">\n");
	printf("<h1><img src=\"/cirrus/cirrus.png\"></h1>\n\
      </div>\n\
      <div role=\"main\" class=\"ui-content\">\n\
      %s<br>\n\
<a href=\"%s\" data-role=\"button\" data-icon=\"back\" data-theme=\"c\">%s</a><br><br>\n",reponse,lienretour,textebouton);
	printf("</div>\n\
<div data-role=\"footer\" class=\"ui-bar\" data-position=\"fixed\" data-theme=\"b\">\n\
<h4>Cirrus - Cloud privé</h4>\n\
</div>\n\
</div>\n\
</body>\n\
</html>\n");
	}
else
	{
	printf("Content-Type: text/html\n\n\
	<!doctype html>\n\
<html>\n\
<title>Cirrus</title>\n\
<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n\
<meta charset=\"utf-8\">\n\
<link rel=\"stylesheet\" href=\"/cirrus/themes/ohm.min.css\" />\n\
<link rel=\"stylesheet\" href=\"/cirrus/themes/jquery.mobile.icons.min.css\" />\n\
<link rel=\"stylesheet\" href=\"/jq/jquery.mobile.structure-1.4.5.min.css\" />\n\
</head>\n\
<body>\n\
Upload d'un fichier vers votre cloud :<br><br>\n\
<form action=\"/cgi-bin/cirrus_upload.cgi\" method=\"post\" enctype=\"multipart/form-data\">\n\
	<input type=\"hidden\" name=\"txt-nom\" id=\"txt-nom\" value=\"%s\">\n\
   <input type=\"hidden\" name=\"txt-password\" id=\"txt-password\" value=\"%s\">\n\
   <input type=\"hidden\" name=\"txt-code\" id=\"txt-code\" value=\"%s\">\n\
   <input type=\"hidden\" name=\"txt-type\" id=\"txt-type\" value=\"%s\">\n\
	<input type=\"file\" name=\"file\">\n\
	<input type=\"submit\">\n\
</form>\n\
</body>\n\
</html>\n",nom,mdp,code,mode);
	}
}