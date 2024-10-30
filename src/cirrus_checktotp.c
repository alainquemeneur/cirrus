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
int i,start,success;
char *query,*reponse,*lienretour,*textebouton,*totpsecret;
char *nom,*mdp;

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
lienretour=(char*)malloc(100);
textebouton=(char*)malloc(20);
totpsecret=(char*)malloc(40);
reponse=(char*)malloc(200);
query=(char*)malloc(20000+query_size);
get_chaine(envoi,2,nom);
tamb(nom);
get_chaine(envoi,1,mdp);
tamb(mdp);
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
				sprintf(query,"select totpsecret from compte where nom=\'%s\';",nom);
				db_query(handler,query);
				db_getvalue(result,0,0,totpsecret,40);
				db_clear_result(result);
				db_close(handler);
				if(strcmp(totpsecret,"")==0) success=1;
				else success=2;
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
      <h1><img src=\"/cirrus/cirrus.png\"></h1>\n\
      </div>\n\
      <div role=\"main\" class=\"ui-content\"><br>\n");
if(success==0) printf("%s\n\
      <a href=\"/cirrus/index.html\" class=\"ui-btn ui-btn-c ui-corner-all mc-top-margin-1-5\">Ressayez</a>",reponse);
if(success==1) printf("Votre compte n'est pas protégé par un deuxième facteur d'authentification, ce qui améliorerait pourtant sa sécurité. Vous pouvez activer cette fonctionnalité depuis la page de votre profil<br><a href=\"/cirrus/2fa.html\">En savoir plus</a><br>\n\
						<form action=\"/cgi-bin/cirrus_folders.cgi\" method=\"post\" id=\"form-connect\" data-transition=\"none\" data-rel=\"dialog\">\n\
                  <input type=\"hidden\" name=\"txt-nom\" id=\"txt-nom\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-password\" id=\"txt-password\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-code\" id=\"txt-code\" value=\"\">\n\
                  <input type=\"hidden\" name=\"txt-enter\" id=\"txt-enter\" value=\"n\">\n\
                  <button type=\"submit\" data-mini=\"false\" data-inline=\"false\" data-theme=\"c\" data-icon=\"action\" class=\"ui-shadow\">Accéder à votre cloud</button>\n\
                  </form>\n",nom,mdp);
if(success==2) 
	{
	printf("<form autocomplete=\"off\" action=\"/cgi-bin/cirrus_folders.cgi\" method=\"post\" id=\"form\" data-transition=\"none\" data-rel=\"dialog\">\n\
            <input type=\"hidden\" name=\"txt-nom\" id=\"txt-nom\" value=\"%s\">\n\
            <input type=\"hidden\" name=\"txt-password\" id=\"txt-password\" value=\"%s\">\n\
            <div data-role=\"fieldcontain\">\n\
               <label for=\"txt-code\">Code de vérification</label>\n\
               <input type=\"text\" name=\"txt-code\" maxlength=\"6\" id=\"txt-code\" value=\"\">\n\
            </div>\n\
            <input type=\"hidden\" name=\"txt-enter\" id=\"txt-enter\" value=\"y\">\n\
            <button type=\"submit\" data-mini=\"false\" data-inline=\"false\" data-theme=\"c\" data-icon=\"action\">Accéder à votre cloud</button>\n\
         </form>\n",nom,mdp);
	sprintf(reponse,"rm /var/www/html/cirrus/qr/qr%s.png",nom);
	system(reponse);
	}
printf("</div>\n\
<div data-role=\"footer\" class=\"ui-bar\" data-position=\"fixed\" data-theme=\"b\">\n\
<h4>Cirrus - Cloud privé</h4>\n\
</div>\n\
</div>\n\
</body>\n\
</html>\n");
}