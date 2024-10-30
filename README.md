# cirrus

Cirrus : Cloud auto-hébergé

Cirrus est un cloud privé auto-hébergé qui permet de stocker et partager l'URL de photos, mp3, fichiers et pages web (sauvegardées par l'extension SAVE PAGES WE sur les navigateurs). C'est le compagnon idéal de XPresso pour faire du blogging. Les quotas sont gérés

J'ai programmé ce logiciel pour répondre à mes besoins.

Techniquement, il s'agit d'une webapp installable n'importe quel OS (Windows, Linux, Mac, iOS, Android), mais prévue pour fonctionner surtout sous iOS ou Android. Le front end est programmé en HTML 5 + toolkit javascript JQuery Mobile. Le back end est constitué de scripts CGI écrits en langage C, traitant les données en interface avec une base de données MySQL ou MariaDB.

Ce programme ne peut fonctionner sans qrencode (pour la génération des QR codes 2FA)

<strong><u>Installation</u></strong>

Sur un serveur Linux, installer NginX, activer le https sur votre domaine, et <a href="https://techexpert.tips/nginx/nginx-shell-script-cgi/">activez les scripts CGI</a> sur votre serveur NginX

Installez cirrus dans /home/votre_repertoire/cirrus

Intégrez votre cirrus dans votre serveur web :
sudo ln -s /home/votre_repertoire/cirrus /var/www/html/cirrus

Installez JQuery Mobile dans /var/www/html/jq :
sudo cp jq.tgz /var/www/html
cd /var/www/html
sudo tar -zxvf ./jq.tgz

Désormais votre webapp est accessible à l'URL : https://votre_serveur/cirrus

Installez qrencode pour être en mesure de générer les QR codes nécessaires pour le 2FA Google Authenticator:
sudo apt-get install qrencode

Assurez vous des bons droits sur les répertoires qr et storage:
cd /home/votre_repertoire/cirrus
chmod 777 qr storage

Il faut maintenant préparer la base de données MySQL :
cd /home/votre_repertoire/cirrus/build
mysql -u root -p mysql
create database cirrus;
create user 'cirrus'@'localhost' identified by 'votre_mot_de_passe';
grant all privileges on cirrus.* to 'cirrus'@'localhost';
quit

mysql -u cirrus -p cirrus
source ./install_mysql_tables.sql;
quit

Avant de compiler les scripts CGI, il faut leur indiquer le mot de passe choisi pour l'utilisateur cirrus dans MySQL ainsi que l'URL du serveur sur Internet:
cd /home/votre_repertoire/cirrus/src
Renseigner le mot de passe MySQL que vous avez choisi dans password.h

Dans specific.h remplacez votre_repertoire par votre répertoire et votre_serveur par votre serveur :
#define STORAGE_DIR "/home/votre_repertoire/cirrus/storage"
#define CIRRUS_CONTAIN_DIR "/home/votre_repertoire"
#define WEBSITE "https://votre_serveur"

Il faut maintenant compiler les scripts CGI :
sudo apt install libmysqlclient-dev
cd /home/votre_repertoire/cirrus/src
sudo mkdir /usr/lib/cg-bin
chmod a+x compile*
sudo ./compile.mysql

Si vous utilisez MariaDB plutôt que MySQL compilez à la place par :
sudo ./compile.mariadb

Une fois que les scripts sont compilés et apparus dans /usr/lib/cgi-bin, il faut effacer par sécurité tous les fichiers présents dans /home/votre_repertoire/cirrus/src :
cd /home/votre_repertoire/cirrus/src
rm * (vérifiez bien d'être dans le bon répertoire avant de taper cela !!!)

Connectez-vous à https://votre_serveur/cirrus et utilisez la webapp

Connectez-vous avec le login Admin, mot de passe admin. Changez le mot de passe admin dans l'interface web.

alain.quemeneur@gmail.com