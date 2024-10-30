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
char *query,*reponse,*lienretour,*textebouton,*totpsecret,*totpcode;
char *nom,*mdp,*code,*quota,*viewmode;

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
viewmode=(char*)malloc(query_size);
lienretour=(char*)malloc(100);
textebouton=(char*)malloc(20);
totpsecret=(char*)malloc(40);
totpcode=(char*)malloc(40);
quota=(char*)malloc(40);
reponse=(char*)malloc(200);
query=(char*)malloc(500+query_size);
get_chaine(envoi,4,nom);
tamb(nom);
get_chaine(envoi,3,mdp);
tamb(mdp);
get_chaine(envoi,2,code);
tamb(code);
get_chaine(envoi,1,viewmode);
tamb(viewmode);
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
					start=0;
					for(i=0;i<db_ntuples(result);i++)
						{
						db_getvalue(result,i,0,reponse,200);
						start += atoi(reponse);
						}
					start=start/1024;
					db_clear_result(result);
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
      <div data-role=\"header\" data-position=\"fixed\" data-theme=\"b\">\n");
if(success==1)
	{
	printf("<div data-role=\"navbar\">\n\
            <ul>\n\
               <li>\n\
               <form target=\"upload\" action=\"/cgi-bin/cirrus_add.cgi\" method=\"post\" id=\"form-add\" data-transition=\"none\" data-rel=\"dialog\">\n\
                  <input type=\"hidden\" name=\"txt-nom\" id=\"txt-nom\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-password\" id=\"txt-password\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-code\" id=\"txt-code\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-mode\" id=\"txt-mode\" value=\"video\">\n\
                  <button type=\"submit\" data-mini=\"false\" data-inline=\"false\" data-theme=\"c\" data-icon=\"plus\" class=\"ui-shadow\">Ajouter</button>\n\
                  </form>\n",nom,mdp,code);
   printf("</li>\n\
               <li>\n\
                  <form action=\"/cgi-bin/cirrus_video_share.cgi\" method=\"post\" id=\"form-share\" data-transition=\"none\" data-rel=\"dialog\">\n\
                  <input type=\"hidden\" name=\"txt-nom\" id=\"txt-nom\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-password\" id=\"txt-password\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-code\" id=\"txt-code\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-viewmode\" id=\"txt-viewmode\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-selection-share\" id=\"txt-selection-share\" value=\"\">\n\
                  <button type=\"submit\" data-mini=\"false\" data-inline=\"false\" data-theme=\"c\" data-icon=\"star\" class=\"ui-shadow\">Partager</button>\n\
                  </form>\n\
               </li>\n\
               <li>\n\
                  <form action=\"/cgi-bin/cirrus_video_suppr.cgi\" method=\"post\" id=\"form-suppr\" data-transition=\"none\" data-rel=\"dialog\">\n\
                  <input type=\"hidden\" name=\"txt-nom\" id=\"txt-nom\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-password\" id=\"txt-password\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-code\" id=\"txt-code\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-viewmode\" id=\"txt-viewmode\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-selection-del\" id=\"txt-selection-del\" value=\"\">\n\
                  <button type=\"submit\" data-mini=\"false\" data-inline=\"false\" data-theme=\"c\" data-icon=\"delete\" class=\"ui-shadow\">Supprimer</button>\n\
                  </form>\n\
               </li>\n\
            </ul>\n\
         </div>\n\
         <h1><img srcset=\"/cirrus/video.png 1.5x\"></h1>\n\
      </div>\n\
      <div role=\"main\" class=\"ui-content\" data-theme=\"b\">\n\
         <p>Espace disque utilisé : %d Go sur %s Go</p>\n\
         <hr>\n",nom,mdp,code,viewmode,nom,mdp,code,viewmode,start,quota);
	sprintf(query,"update compte set videoviewmode=\'%s\' where nom=\'%s\';",viewmode,nom);
	db_query(handler,query);
	db_clear_result(result);
	strcpy(reponse," order by nom asc");
	if(strcmp(viewmode,"noma")==0) strcpy(reponse," order by nom asc");
	if(strcmp(viewmode,"nomd")==0) strcpy(reponse," order by nom desc");
	if(strcmp(viewmode,"datea")==0) strcpy(reponse," order by id asc");
	if(strcmp(viewmode,"dated")==0) strcpy(reponse," order by id desc");
	sprintf(query,"select nom,lien,id from entree where owner=\'%s\' and zone=\'video\'%s;",nom,reponse);
	db_query(handler,query);
	for(i=0;i<db_ntuples(result);i++)
		{
		db_getvalue(result,i,0,reponse,200);
		printf("<strong>%s</strong><br>\n",reponse);
		db_getvalue(result,i,1,reponse,200);
      printf("<video controls src=\"%s\" style=\"height:50vh; max-width:100%%; object-fit: contain;\" controls>Votre navigateur ne supporte pas l'affichage des vidéos</video>\n",reponse);
      db_getvalue(result,i,2,reponse,200);
      printf("<br><button onclick=\"document.getElementById('txt-selection-share').value = '%s';document.getElementById('txt-selection-del').value = '%s';\" data-inline=\"true\" data-theme=\"c\" data-mini=\"true\">Sélectionner</button>\n<hr>\n",reponse,reponse);
      }
   db_clear_result(result);
   db_close(handler);        
   printf("<div data-role=\"fieldcontain\">\n\
         <form action=\"/cgi-bin/cirrus_videos.cgi\" method=\"post\" id=\"form-trin-asc\" data-transition=\"none\" data-rel=\"dialog\">\n\
         <input type=\"hidden\" name=\"txt-nom\" id=\"txt-nom\" value=\"%s\">\n\
         <input type=\"hidden\" name=\"txt-password\" id=\"txt-password\" value=\"%s\">\n\
         <input type=\"hidden\" name=\"txt-code\" id=\"txt-code\" value=\"%s\">\n\
         <input type=\"hidden\" name=\"txt-viewmode\" id=\"txt-viewmode\" value=\"noma\">\n\
         <button type=\"submit\" data-mini=\"true\" data-inline=\"true\" data-theme=\"c\" data-icon=\"carat-u\" class=\"ui-shadow\">Trier par nom (↑)</button>\n\
         </form>\n\
         <form action=\"/cgi-bin/cirrus_videos.cgi\" method=\"post\" id=\"form-trin-desc\" data-transition=\"none\" data-rel=\"dialog\">\n\
         <input type=\"hidden\" name=\"txt-nom\" id=\"txt-nom\" value=\"%s\">\n\
         <input type=\"hidden\" name=\"txt-password\" id=\"txt-password\" value=\"%s\">\n\
         <input type=\"hidden\" name=\"txt-code\" id=\"txt-code\" value=\"%s\">\n\
         <input type=\"hidden\" name=\"txt-viewmode\" id=\"txt-viewmode\" value=\"nomd\">\n\
         <button type=\"submit\" data-mini=\"true\" data-inline=\"true\" data-theme=\"c\" data-icon=\"carat-d\" class=\"ui-shadow\">Trier par nom (↓)</button>\n\
         </form>\n\
         </div>\n\
         <div data-role=\"fieldcontain\">\n\
         <form action=\"/cgi-bin/cirrus_videos.cgi\" method=\"post\" id=\"form-trid-asc\" data-transition=\"none\" data-rel=\"dialog\">\n\
         <input type=\"hidden\" name=\"txt-nom\" id=\"txt-nom\" value=\"%s\">\n\
         <input type=\"hidden\" name=\"txt-password\" id=\"txt-password\" value=\"%s\">\n\
         <input type=\"hidden\" name=\"txt-code\" id=\"txt-code\" value=\"%s\">\n\
         <input type=\"hidden\" name=\"txt-viewmode\" id=\"txt-viewmode\" value=\"datea\">\n\
         <button type=\"submit\" data-mini=\"true\" data-inline=\"true\" data-theme=\"c\" data-icon=\"carat-u\" class=\"ui-shadow\">Trier par date (↑)</button>\n\
         </form>\n\
         <form action=\"/cgi-bin/cirrus_videos.cgi\" method=\"post\" id=\"form-trid-desc\" data-transition=\"none\" data-rel=\"dialog\">\n\
         <input type=\"hidden\" name=\"txt-nom\" id=\"txt-nom\" value=\"%s\">\n\
         <input type=\"hidden\" name=\"txt-password\" id=\"txt-password\" value=\"%s\">\n\
         <input type=\"hidden\" name=\"txt-code\" id=\"txt-code\" value=\"%s\">\n\
         <input type=\"hidden\" name=\"txt-viewmode\" id=\"txt-viewmode\" value=\"dated\">\n\
         <button type=\"submit\" data-mini=\"true\" data-inline=\"true\" data-theme=\"c\" data-icon=\"carat-d\" class=\"ui-shadow\">Trier par date (↓)</button>\n\
         </form>\n\
         </div>\n\
      </div>\n\
      <div data-role=\"footer\" data-position=\"fixed\" data-theme=\"b\">\n\
      <div data-role=\"navbar\">\n\
         <ul>\n\
            <li>\n\
               <form action=\"/cgi-bin/cirrus_folders.cgi\" method=\"post\" id=\"form-folders\" data-transition=\"none\" data-rel=\"dialog\">\n\
               <input type=\"hidden\" name=\"txt-nom\" id=\"txt-nom\" value=\"%s\">\n\
               <input type=\"hidden\" name=\"txt-password\" id=\"txt-password\" value=\"%s\">\n\
               <input type=\"hidden\" name=\"txt-code\" id=\"txt-code\" value=\"%s\">\n\
               <input type=\"hidden\" name=\"txt-enter\" id=\"txt-enter\" value=\"n\">\n\
               <button type=\"submit\" data-mini=\"false\" data-inline=\"false\" data-theme=\"c\" data-icon=\"home\" class=\"ui-shadow\">Retour à l'accueil</button>\n\
               </form>\n\
            </li>\n\
         </ul>\n\
      </div>\n\
         <h4>Cirrus - Cloud privé</h4>\n\
      </div>\n\
   </div>\n\
</body>\n\
</html>\n",nom,mdp,code,nom,mdp,code,nom,mdp,code,nom,mdp,code,nom,mdp,code);
	}
else
	{
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
}