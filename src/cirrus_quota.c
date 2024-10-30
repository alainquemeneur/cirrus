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
char *query,*reponse,*lienretour,*textebouton,*totpsecret,*totpcode;
char *mdp,*code;

envoi=read_POST();
start=strlen(envoi);
for(i=0;i<start;i++)
	{
	if(envoi[i]<32) envoi[i]=0;
	}
query_size=atoi(getenv("CONTENT_LENGTH"));
mdp=(char*)malloc(query_size);
code=(char*)malloc(query_size);
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
	strcpy(query,"select * from compte where nom=\'Admin\';");
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
			sprintf(query,"select * from compte where nom=\'Admin\' and mdp=\'%s\' and actif=\'oui\';",mdp);
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
      		if(succes==1)
      			{
					strcpy(query,"select nom from compte where nom!=\'Admin\' and actif=\'oui\' order by nom asc;");
					db_query(handler,query);
					}
				else db_close(handler);
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
                  <form action=\"/cgi-bin/cirrus_folders.cgi\" method=\"post\" id=\"form-folders\" data-transition=\"none\" data-rel=\"dialog\">\n\
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
if(succes==0) printf("%s\n",reponse);
else
	{
	printf("<p style=\"text-align:center\"><strong>Gestion des quotas</strong></p>\n");
	if(db_ntuples(result)>0)
		{
		printf("<form autocomplete=\"off\" action=\"/cgi-bin/cirrus_changequota.cgi\" method=\"post\" id=\"form-quota\" data-transition=\"none\" data-rel=\"dialog\">\n\
                  <input type=\"hidden\" name=\"txt-password\" id=\"txt-password\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-code\" id=\"txt-code\" value=\"%s\">\n\
                  <div data-role=\"fieldcontain\">\n\
               	<label for=\"txt-quota\">Nouveau quota</label>\n\
                  <input type=\"number\" name=\"txt-quota\" id=\"txt-quota\" value=\"\">\n\
                  </div>\n\
                  <fieldset data-role=\"controlgroup\">\n\
               	<legend>Comptes sur lequel appliquer ce quota</legend>\n",mdp,code);
		for(i=0;i<db_ntuples(result);i++)
			{
			db_getvalue(result,i,0,reponse,200);
			if(i==0) printf("<input type=\"radio\" name=\"radio-choice-r\" id=\"radio-choice-r%d\" value=\"%s\" data-theme=\"c\" checked=\"checked\">\n\
               <label for=\"radio-choice-r%d\">%s</label>\n",i,reponse,i,reponse);
			else printf("<input type=\"radio\" name=\"radio-choice-r\" id=\"radio-choice-r%d\" value=\"%s\" data-theme=\"c\">\n\
               <label for=\"radio-choice-r%d\">%s</label>\n",i,reponse,i,reponse);
			}
		printf("</fieldset>\n<input type=\"submit\" data-mini=\"false\" data-inline=\"false\" data-theme=\"c\" data-icon=\"cloud\" value=\"Appliquer le quota\">\n</form>\n");
		}
	db_clear_result(result);
	db_close(handler);
	}
printf("</div>\n\
<div data-role=\"footer\" class=\"ui-bar\" data-position=\"fixed\" data-theme=\"b\">\n\
<h4>Cirrus - Cloud privé</h4>\n\
</div>\n\
</div>\n\
</body>\n\
</html>\n");
}
