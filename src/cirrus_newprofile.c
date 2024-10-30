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
int i,start;
char *query,*reponse,*lienretour,*textebouton,*storage_dir;
char *nom,*mdp1,*mdp2;

envoi=read_POST();
start=strlen(envoi);
for(i=0;i<start;i++)
	{
	if(envoi[i]<32) envoi[i]=0;
	}
query_size=atoi(getenv("CONTENT_LENGTH"));
nom=(char*)malloc(query_size);
mdp1=(char*)malloc(query_size);
mdp2=(char*)malloc(query_size);
reponse=(char*)malloc(200);
lienretour=(char*)malloc(100);
textebouton=(char*)malloc(20);
storage_dir=(char*)malloc(256);
strcpy(storage_dir,STORAGE_DIR);
query=(char*)malloc(20000+query_size);
get_chaine(envoi,3,nom);
tamb(nom);
get_chaine(envoi,2,mdp2);
tamb(mdp2);
get_chaine(envoi,1,mdp1);
tamb(mdp1);
if(strcmp(mdp1,mdp2)!=0) 
		{
		strcpy(reponse,"Erreur : Création de compte impossible, les deux mots de passe ne correspondent pas ...");
		strcpy(lienretour,"/cirrus/cirrus_newprofile.html");
		strcpy(textebouton,"Réessayer");
		}
else
		{
		if((handler=db_opendatabase("cirrus","localhost","cirrus",PASSWORD))==NULL)
			{
			strcpy(reponse,"Erreur : Impossible d'accéder à la base de données");
			strcpy(lienretour,"/cirrus/cirrus_newprofile.html");
			strcpy(textebouton,"Réessayer");
			}
		else
			{
			sprintf(query,"select * from compte where nom=\'%s\';",nom);
			db_query(handler,query);
			if(db_ntuples(result)!=0)
				{
				strcpy(reponse,"Erreur : Un profil à ce nom existe déjà ...");
				strcpy(lienretour,"/cirrus/cirrus_newprofile.html");
				strcpy(textebouton,"Réessayer");
				db_clear_result(result);
				db_close(handler);
				}
			else
				{
				db_clear_result(result);
				sprintf(query,"insert into compte values (\'%s\',\'%s\',\'non\',\'\',\'\',5,\'noma\',\'dated\',\'noma\',\'noma\',\'noma\');",nom,mdp1);
				db_query(handler, query);
				sprintf(reponse,"mkdir %s/%s; mkdir %s/%s/musique; mkdir %s/%s/document; mkdir %s/%s/video; mkdir %s/%s/image; mkdir %s/%s/pageweb;",storage_dir,nom,storage_dir,nom,storage_dir,nom,storage_dir,nom,storage_dir,nom,storage_dir,nom);
				system(reponse);
				sprintf(reponse,"chmod 777 %s/%s; chmod 777 %s/%s/musique; chmod 777 %s/%s/document; chmod 777 %s/%s/video; chmod 777 %s/%s/image; chmod 777 %s/%s/pageweb;",storage_dir,nom, storage_dir,nom,storage_dir,nom,storage_dir,nom,storage_dir,nom,storage_dir,nom);
				system(reponse);
				strcpy(reponse,"Félicitations ! Votre compte a bien été créé. Il ne sera utilisable que lorsqu'il aura été activé par l'administrateur. Bienvenue sur Cirrus, le cloud privé");
				strcpy(lienretour,"/cirrus/index.html");
				strcpy(textebouton,"Se connecter");
				db_clear_result(result);
				db_close(handler);
				}
			}
		}
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
