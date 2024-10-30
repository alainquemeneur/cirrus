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
#include "totp.c"

int main()
{
char *envoi;
int i,start,success,nb_musique,nb_image,nb_video,nb_pageweb,nb_document;
char *query,*reponse,*lienretour,*textebouton,*totpsecret,*totpcode;
char *nom,*mdp,*code,*enter,*quota,*musicviewmode, *imageviewmode, *videoviewmode, *documentviewmode, *pagewebviewmode;

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
enter=(char*)malloc(query_size);
reponse=(char*)malloc(200);
lienretour=(char*)malloc(100);
textebouton=(char*)malloc(20);
totpsecret=(char*)malloc(40);
totpcode=(char*)malloc(40);
quota=(char*)malloc(40);
musicviewmode=(char*)malloc(40);
imageviewmode=(char*)malloc(40);
videoviewmode=(char*)malloc(40);
documentviewmode=(char*)malloc(40);
pagewebviewmode=(char*)malloc(40);
query=(char*)malloc(20000+query_size);
get_chaine(envoi,4,nom);
tamb(nom);
get_chaine(envoi,3,mdp);
tamb(mdp);
get_chaine(envoi,2,code);
tamb(code);
get_chaine(envoi,1,enter);
tamb(enter);
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
				success=1;
				if(strcmp(totpsecret,"")!=0)
					{
					if(strcmp(enter,"y")==0)
						{
						if(checkcode(totpsecret,code)==0)
							{
							sprintf(query,"update compte set totpcode=\'%s\' where nom=\'%s\';",code,nom);
							db_query(handler,query);
							db_clear_result(result);
							}
						else 
							{
							strcpy(reponse,"Erreur : Code de vérification erroné ...");
							strcpy(lienretour,"/cirrus/index.html");
							strcpy(textebouton,"Réessayer");
							success=0;
							sleep(2);
							}
						}
					else
						{
						if(strcmp(totpcode,code)!=0)
							{
							strcpy(reponse,"Erreur : Code de vérification erroné ...");
							strcpy(lienretour,"/cirrus/index.html");
							strcpy(textebouton,"Réessayer");
							success=0;
							sleep(2);
							}
						}
					}
				if(success==1)
					{
					sprintf(query,"select musicviewmode,videoviewmode,imageviewmode,documentviewmode,pagewebviewmode from compte where nom=\'%s\';",nom);
					db_query(handler,query);
					db_getvalue(result,0,0,musicviewmode,40);
					db_getvalue(result,0,1,videoviewmode,40);
					db_getvalue(result,0,2,imageviewmode,40);
					db_getvalue(result,0,3,documentviewmode,40);
					db_getvalue(result,0,4,pagewebviewmode,40);
					db_clear_result(result);
					sprintf(query,"select * from entree where owner=\'%s\' and zone=\'musique\';",nom);
					db_query(handler,query);
					nb_musique=db_ntuples(result);
					db_clear_result(result);
					sprintf(query,"select * from entree where owner=\'%s\' and zone=\'video\';",nom);
					db_query(handler,query);
					nb_video=db_ntuples(result);
					db_clear_result(result);
					sprintf(query,"select * from entree where owner=\'%s\' and zone=\'image\';",nom);
					db_query(handler,query);
					nb_image=db_ntuples(result);
					db_clear_result(result);
					sprintf(query,"select * from entree where owner=\'%s\' and zone=\'document\';",nom);
					db_query(handler,query);
					nb_document=db_ntuples(result);
					db_clear_result(result);
					sprintf(query,"select * from entree where owner=\'%s\' and zone=\'pageweb\';",nom);
					db_query(handler,query);
					nb_pageweb=db_ntuples(result);
					db_clear_result(result);
					sprintf(query,"select quota from compte where nom=\'%s\';",nom);
					db_query(handler,query);
					db_getvalue(result,0,0,quota,40);
					db_clear_result(result);
					sprintf(query,"select taille from entree where owner=\'%s\';",nom);
					db_query(handler,query);
					start=0;
					for(i=0;i<db_ntuples(result);i++)
						{
						db_getvalue(result,i,0,reponse,200);
						start += atoi(reponse);
						}
					start=start/1024;
					db_clear_result(result);
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
      <div data-role=\"header\" data-position=\"fixed\" data-theme=\"b\">");
if(success==1)
	{
	printf("<div data-role=\"navbar\">\n\
            <ul>\n\
               <li>\n\
               <form action=\"/cgi-bin/cirrus_editprofile.cgi\" method=\"post\" id=\"form-profile\" data-transition=\"none\" data-rel=\"dialog\">\n\
                  <input type=\"hidden\" name=\"txt-nom\" id=\"txt-nom\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-password\" id=\"txt-password\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-code\" id=\"txt-code\" value=\"%s\">\n\
                  <button type=\"submit\" data-mini=\"false\" data-inline=\"false\" data-theme=\"c\" data-icon=\"user\" class=\"ui-shadow\">Mon compte</button>\n\
                  </form>\n\
               </li>\n\
            </ul>\n\
            </div>\n\
            <h1><img src=\"/cirrus/cirrus.png\"></h1>\n\
      </div>\n\
      <div role=\"main\" class=\"ui-content\" data-theme=\"b\">\n",nom,mdp,code);
     
      if(strcmp(nom,"Admin")==0) 
      	{
      	printf("<form action=\"/cgi-bin/cirrus_comptes.cgi\" method=\"post\" id=\"form-comptes\" data-transition=\"none\" data-rel=\"dialog\">\n\
                  <input type=\"hidden\" name=\"txt-password\" id=\"txt-password\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-code\" id=\"txt-code\" value=\"%s\">\n\
                  <input type=\"submit\" data-mini=\"true\" data-inline=\"true\" data-theme=\"c\" data-icon=\"user\" value=\"Gérer les comptes\"></form>\n",mdp,code);
      	printf("<form action=\"/cgi-bin/cirrus_stats.cgi\" method=\"post\" id=\"form-stats\" data-transition=\"none\" data-rel=\"dialog\">\n\
                  <input type=\"hidden\" name=\"txt-password\" id=\"txt-password\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-code\" id=\"txt-code\" value=\"%s\">\n\
                  <input type=\"submit\" data-mini=\"true\" data-inline=\"true\" data-theme=\"c\" data-icon=\"user\" value=\"Statistiques\"></form>\n",mdp,code);
      	printf("<form action=\"/cgi-bin/cirrus_quota.cgi\" method=\"post\" id=\"form-quota\" data-transition=\"none\" data-rel=\"dialog\">\n\
                  <input type=\"hidden\" name=\"txt-password\" id=\"txt-password\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-code\" id=\"txt-code\" value=\"%s\">\n\
                  <input type=\"submit\" data-mini=\"true\" data-inline=\"true\" data-theme=\"c\" data-icon=\"user\" value=\"Gérer les quotas\"></form>\n",mdp,code);
      	}
      printf("<p>Espace disque utilisé : %d Go sur %s Go</p><br>\n\
         <ul data-role=\"listview\">\n\
            <li>\n\
               <form action=\"/cgi-bin/cirrus_musiques.cgi\" method=\"post\" id=\"form-music\" data-transition=\"none\" data-rel=\"dialog\">\n\
                  <div data-role=\"fieldcontain\">\n\
                  <input type=\"hidden\" name=\"txt-nom\" id=\"txt-nom\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-password\" id=\"txt-password\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-code\" id=\"txt-code\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-viewmode\" id=\"txt-code\" value=\"%s\">\n\
                  <button type=\"submit\" data-mini=\"false\" data-inline=\"true\" data-theme=\"c\"><span style=\"vertical-align: middle;\"> <img src=\"/cirrus/music.png\"></span> Musique</button>\n\
                  <span class=\"ui-li-count\">%d</span>\n\
                  </div>\n\
               </form>\n\
            </li>\n", start, quota, nom, mdp, code, musicviewmode, nb_musique);
      printf("<li>\n\
               <form action=\"/cgi-bin/cirrus_images.cgi\" method=\"post\" id=\"form-images\" data-transition=\"none\" data-rel=\"dialog\">\n\
                  <div data-role=\"fieldcontain\">\n\
                  <input type=\"hidden\" name=\"txt-nom\" id=\"txt-nom\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-password\" id=\"txt-password\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-code\" id=\"txt-code\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-viewmode\" id=\"txt-code\" value=\"%s\">\n\
                  <button type=\"submit\" data-mini=\"false\" data-inline=\"true\" data-theme=\"c\"><span style=\"vertical-align: middle;\"> <img src=\"/cirrus/image.png\"></span> Photos</button>\n\
                  <span class=\"ui-li-count\">%d</span>\n\
                  </div>\n\
               </form>\n\
            </li>\n", nom,mdp,code,imageviewmode,nb_image);
      printf("<li>\n\
               <form action=\"/cgi-bin/cirrus_videos.cgi\" method=\"post\" id=\"form-videos\" data-transition=\"none\" data-rel=\"dialog\">\n\
                  <div data-role=\"fieldcontain\">\n\
                  <input type=\"hidden\" name=\"txt-nom\" id=\"txt-nom\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-password\" id=\"txt-password\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-code\" id=\"txt-code\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-viewmode\" id=\"txt-code\" value=\"%s\">\n\
                  <button type=\"submit\" data-mini=\"false\" data-inline=\"true\" data-theme=\"c\"><span style=\"vertical-align: middle;\"> <img src=\"/cirrus/video.png\"></span>Vidéos</button>\n\
                  <span class=\"ui-li-count\">%d</span>\n\
                  </div>\n\
               </form>\n\
            </li>\n",nom,mdp,code,videoviewmode,nb_video);
      printf("<li>\n\
               <form action=\"/cgi-bin/cirrus_documents.cgi\" method=\"post\" id=\"form-docs\" data-transition=\"none\" data-rel=\"dialog\">\n\
                  <div data-role=\"fieldcontain\">\n\
                  <input type=\"hidden\" name=\"txt-nom\" id=\"txt-nom\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-password\" id=\"txt-password\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-code\" id=\"txt-code\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-viewmode\" id=\"txt-code\" value=\"%s\">\n\
                  <button type=\"submit\" data-mini=\"false\" data-inline=\"true\" data-theme=\"c\"><span style=\"vertical-align: middle;\"> <img src=\"/cirrus/document.png\"></span> Documents</button>\n\
                  <span class=\"ui-li-count\">%d</span>\n\
                  </div>\n\
               </form>\n\
            </li>\n",nom,mdp,code,documentviewmode,nb_document);
      printf("<li>\n\
               <form action=\"/cgi-bin/cirrus_pagesweb.cgi\" method=\"post\" id=\"form-pagesweb\" data-transition=\"none\" data-rel=\"dialog\">\n\
                  <div data-role=\"fieldcontain\">\n\
                  <input type=\"hidden\" name=\"txt-nom\" id=\"txt-nom\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-password\" id=\"txt-password\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-code\" id=\"txt-code\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-viewmode\" id=\"txt-code\" value=\"%s\">\n\
                  <button type=\"submit\" data-mini=\"false\" data-inline=\"true\" data-theme=\"c\"><span style=\"vertical-align: middle; horizontal-align : left;\"> <img src=\"/cirrus/pageweb.png\"></span> Pages web</button>\n\
                  <span class=\"ui-li-count\">%d</span>\n\
                  </div>\n\
               </form>\n\
            </li>\n\
         </ul>\n",nom,mdp,code,pagewebviewmode,nb_pageweb);
	/*if(strcmp(nom,"Admin")==0) printf("<hr><form action=\"/cgi-bin/cirrus_confirmnitrater.cgi\" method=\"post\" id=\"form-nitrater\" data-transition=\"none\" data-rel=\"dialog\">\n\
                  <input type=\"hidden\" name=\"txt-password\" id=\"txt-password\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-code\" id=\"txt-code\" value=\"%s\">\n\
                  <input type=\"submit\" data-mini=\"true\" data-inline=\"true\" data-theme=\"c\" data-icon=\"alert\" value=\"Nitrater la base de données\"></form>\n",mdp,code);*/
	}
else
	{
	printf("<h1><img src=\"/cirrus/cirrus.png\"></h1>\n\
      </div>\n\
      <div role=\"main\" class=\"ui-content\">\n\
      %s<br>\n\
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
