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
int i,start,succes,fd,nb;
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
         		sprintf(reponse,"openssl rand 10 | base32 > \'/tmp/cirrustotp%s\'",nom);
					system(reponse);
					sprintf(reponse,"/tmp/cirrustotp%s",nom);
					fd=open(reponse,O_RDONLY);
					nb=read(fd,reponse,100);
					close(fd);
					reponse[nb]=0;
					strcpy(lienretour,reponse);
					sprintf(reponse,"echo \'otpauth://totp/Cirrus (%s)?secret=%s\' | qrencode -t png -o \'/var/www/html/cirrus/qr/qr%s.png\'",nom,lienretour,nom);
					system(reponse);
         		sprintf(query,"update compte set totpsecret=\'%s\', totpcode=\'\' where nom=\'%s\';",lienretour,nom);
         		db_query(handler,query);
         		db_clear_result(result);
         		strcpy(code,"");
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
      <div data-role=\"navbar\">\n\
            <ul>\n\
               <li>\n\
                  <a href=\"/cirrus/index.html\" data-role=\"button\" data-icon=\"back\" data-theme=\"c\">Reconnexion</a>\n\
               </li>\n\
            </ul>\n\
         </div>\n\
         <h1><img src=\"/cirrus/cirrus.png\"></h1>\n\
      </div>\n\
      <div role=\"main\" class=\"ui-content\">\n");
if(succes==1)
	{
	printf("L'authentification 2FA a été activée pour protéger votre compte. Merci de bien vouloir scanner le QR code ci-dessous avec l'application Google Authenticator sur votre smartphone, ou entrez y manuellement le code secret qui vous a été attribué.\n\
		<br><img src=\"/cirrus/qr/qr%s.png\"><br>Code secret : %s",nom,lienretour);
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
