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
int i,start,succes;
char *query,*res,*reponse,*lienretour,*textebouton,*totpsecret,*totpcode;
char *nom,*mdp,*code;

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
res=(char*)malloc(100);
reponse=(char*)malloc(200);
lienretour=(char*)malloc(100);
textebouton=(char*)malloc(20);
totpsecret=(char*)malloc(40);
totpcode=(char*)malloc(40);
query=(char*)malloc(20000+query_size);
get_chaine(envoi,3,nom);
tamb(nom);
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
      		if(strcmp(totpsecret,"")==0) succes=2;
      		else
         		{
         		if(strcmp(totpcode,code)==0) succes=3;
         		else
            		{
            		strcpy(reponse,"Erreur : Code de vérification erroné ...");
            		strcpy(lienretour,"/cirrus/index.html");
            		strcpy(textebouton,"Réessayer");
            		sleep(2);
            		}
         		}
				db_close(handler);
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
      <div role=\"main\" class=\"ui-content\">\n");
if(succes==2) 
	{
	printf("<form action=\"/cgi-bin/cirrus_confirmactivetotp.cgi\" method=\"post\" id=\"form-totp\" data-transition=\"none\" data-rel=\"dialog\">\n\
		<input type=\"hidden\" name=\"txt-nom\" id=\"txt-nom\" value=\"%s\">\n\
        <input type=\"hidden\" name=\"txt-password\" id=\"txt-password\" value=\"%s\">\n\
        <input type=\"hidden\" name=\"txt-code\" id=\"txt-code\" value=\"%s\">\n\
		<button type=\"submit\" data-theme=\"c\" data-mini=\"true\" data-inline=\"true\" data-icon=\"lock\">Activer l'authentification 2FA</button>\n\
		</form>",nom,mdp,code);
	succes=1;
	}
if(succes==3) 
	{
	printf("<form action=\"/cgi-bin/cirrus_confirmdesactivetotp.cgi\" method=\"post\" id=\"form-totp\" data-transition=\"none\" data-rel=\"dialog\">\n\
		<input type=\"hidden\" name=\"txt-nom\" id=\"txt-nom\" value=\"%s\">\n\
        <input type=\"hidden\" name=\"txt-password\" id=\"txt-password\" value=\"%s\">\n\
        <input type=\"hidden\" name=\"txt-code\" id=\"txt-code\" value=\"%s\">\n\
		<button type=\"submit\" data-theme=\"c\" data-mini=\"true\" data-inline=\"true\" data-icon=\"lock\">Désactiver l'authentification 2FA</button>\n\
		</form>",nom,mdp,code);
	succes=1;
	}
if(succes==1)
	{
	printf("<p style=\"text-align:center\"><strong>Mon compte</strong></p>\n\
               <form autocomplete=\"off\" action=\"/cgi-bin/cirrus_updateprofile.cgi\" method=\"post\" id=\"form-update\" data-transition=\"none\" data-rel=\"dialog\">\n\
                  <input type=\"hidden\" name=\"txt-nom\" id=\"txt-nom\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-password\" id=\"txt-password\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-code\" id=\"txt-code\" value=\"%s\">\n\
                  <div data-role=\"fieldcontain\">\n\
                  <label for=\"txt-nom\">Nom *</label>\n\
                  <input type=\"text\" name=\"txt-nom\" id=\"txt-nom\" maxlength=\"99\" value=\"%s\" disabled>\n\
                  </div>\n\
                  <div data-role=\"fieldcontain\">\n\
                  <label for=\"txt-oldpassword\">Mot de passe (actuel)</label>\n\
                  <input type=\"password\" name=\"txt-oldpassword\" maxlength=\"99\" id=\"txt-oldpassword\" value=\"\">\n\
                  </div>\n\
                  <div data-role=\"fieldcontain\">\n\
                  <label for=\"txt-password\">Mot de passe (nouveau)</label>\n\
                  <input type=\"password\" name=\"txt-password\" maxlength=\"99\" id=\"txt-password\" value=\"\">\n\
                  </div>\n\
                  <div data-role=\"fieldcontain\">\n\
                  <label for=\"txt-confirm\">Confirmation du mot de passe</label>\n\
                  <input type=\"password\" name=\"txt-confirm\" maxlength=\"99\" id=\"txt-confirm\" value=\"\">\n\
                  </div>\n\
                  <input type=\"submit\" data-mini=\"false\" data-inline=\"false\" data-theme=\"c\" data-icon=\"action\" value=\"Mettre à jour mon mot de passe\">\n\
                  </form>\n\
                  <form action=\"/cgi-bin/cirrus_folders.cgi\" method=\"post\" id=\"form-folders\" data-transition=\"none\" data-rel=\"dialog\">\n\
                  <input type=\"hidden\" name=\"txt-nom\" id=\"txt-nom\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-password\" id=\"txt-password\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-code\" id=\"txt-code\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-enter\" id=\"txt-enter\" value=\"n\">\n\
                  <input type=\"submit\" data-mini=\"false\" data-inline=\"false\" data-theme=\"c\" data-icon=\"back\" value=\"Annuler\">\n\
                  </form>\n",nom,mdp,code,nom,nom,mdp,code);
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
