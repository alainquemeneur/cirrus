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
#include "specific.h"

int main()
{
char *envoi;
int i,start,success;
char *query,*reponse,*lienretour,*textebouton,*storage_dir,*totpsecret,*totpcode;
char *nom,*mdp,*code,*viewmode,*selection;

envoi=read_POST();
start=strlen(envoi);
for(i=0;i<start;i++)
	{
	if(envoi[i]<32) envoi[i]=0;
	}
query_size=atoi(getenv("CONTENT_LENGTH"));
nom=(char*)malloc(query_size);
mdp=(char*)malloc(query_size);
code=(char*)malloc(query_size);
viewmode=(char*)malloc(query_size);
selection=(char*)malloc(query_size);
reponse=(char*)malloc(200);
lienretour=(char*)malloc(100);
textebouton=(char*)malloc(20);
storage_dir=(char*)malloc(256);
totpsecret=(char*)malloc(40);
totpcode=(char*)malloc(40);
strcpy(storage_dir,WEBSITE);
query=(char*)malloc(20000+query_size);
get_chaine(envoi,5,nom);
tamb(nom);
get_chaine(envoi,4,mdp);
tamb(mdp);
get_chaine(envoi,3,code);
tamb(code);
get_chaine(envoi,2,viewmode);
tamb(viewmode);
get_chaine(envoi,1,selection);
tamb(selection);
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
         			if(strcmp(selection,"")!=0) 
         				{
         				sprintf(query,"select lien from entree where owner=\'%s\' and id=\'%s\';",nom,selection);
         				db_query(handler,query);
         				if(db_ntuples(result)>0) db_getvalue(result,0,0,reponse,200);
         				else
         					{
         					success=0;
         					strcpy(reponse,"Erreur : fichier inconnu");
         					}
         				db_clear_result(result);
         				}
         			else
         				{
         				success=2;
         				}
         			}
         		db_close(handler);
         		}
         	}
	    }
    }
if(success==0)
	{
	printf("Content-Type: text/html\n\n\
<!doctype html>\n\
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
<div data-role=\"header\" data-theme=\"b\">\n\
<h1><img src=\"/cirrus/cirrus.png\"></h1>\n\
</div>\n\
<div role=\"main\" class=\"ui-content\">\n\
%s<br>\n\
<a href=\"%s\" data-role=\"button\" data-icon=\"back\" data-theme=\"c\">%s</a><br><br>\n\
</div>\n\
<div data-role=\"footer\" class=\"ui-bar\" data-position=\"fixed\" data-theme=\"b\">\n\
<h4>Cirrus - Cloud privé</h4>\n\
</div>\n\
</div>\n\
</body>\n\
</html>",reponse,lienretour,textebouton);
	}
if(success==1)
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
      <div data-role=\"header\" data-position=\"fixed\" data-theme=\"b\">\n\
      <div data-role=\"navbar\">\n\
            <ul>\n\
               <li>\n\
               <form action=\"/cgi-bin/cirrus_images.cgi\" method=\"post\" id=\"form-add\" data-transition=\"none\" data-rel=\"dialog\">\n\
                  <input type=\"hidden\" name=\"txt-nom\" id=\"txt-nom\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-password\" id=\"txt-password\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-code\" id=\"txt-code\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-view\" id=\"txt-view\" value=\"%s\">\n\
                  <button type=\"submit\" data-mini=\"false\" data-inline=\"false\" data-theme=\"c\" data-icon=\"back\" class=\"ui-shadow\">Retour</button>\n\
                  </form>\n\
               </li>\n\
            </ul>\n\
       	</div>\n\
       	<h1><img srcset=\"/cirrus/image.png 1.5x\"></h1>\n\
      </div>\n\
      <div role=\"main\" class=\"ui-content\" data-theme=\"b\">\n\
      <br><br>Lien vers ce fichier : <a href=\"%s%s\" target=\"lien\">%s%s</a><br><br>\n\
      </div>\n\
      <div data-role=\"footer\" class=\"ui-bar\" data-position=\"fixed\" data-theme=\"b\">\n\
<h4>Cirrus - Cloud privé</h4>\n\
</div>\n\
</div>\n\
</body>\n\
</html>",nom,mdp,code,viewmode,storage_dir,reponse,storage_dir,reponse);
	}
if(success==2)
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
      <div data-role=\"header\" data-position=\"fixed\" data-theme=\"b\">\n\
      <div data-role=\"navbar\">\n\
            <ul>\n\
               <li>\n\
               <form action=\"/cgi-bin/cirrus_images.cgi\" method=\"post\" id=\"form-add\" data-transition=\"none\" data-rel=\"dialog\">\n\
                  <input type=\"hidden\" name=\"txt-nom\" id=\"txt-nom\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-password\" id=\"txt-password\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-code\" id=\"txt-code\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-view\" id=\"txt-view\" value=\"%s\">\n\
                  <button type=\"submit\" data-mini=\"false\" data-inline=\"false\" data-theme=\"c\" data-icon=\"back\" class=\"ui-shadow\">Retour</button>\n\
                  </form>\n\
               </li>\n\
            </ul>\n\
       	</div>\n\
       	<h1><img srcset=\"/cirrus/image.png 1.5x\"></h1>\n\
      </div>\n\
      <div role=\"main\" class=\"ui-content\" data-theme=\"b\">\n\
      <br><br>Fichier inconnu. Veuillez séléctionner un fichier sur la page précédente\n\
      </div>\n\
      <div data-role=\"footer\" class=\"ui-bar\" data-position=\"fixed\" data-theme=\"b\">\n\
<h4>Cirrus - Cloud privé</h4>\n\
</div>\n\
</div>\n\
</body>\n\
</html>",nom,mdp,code,viewmode);
	}
}