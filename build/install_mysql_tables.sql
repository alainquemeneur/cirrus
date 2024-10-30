CREATE TABLE compte (nom text,mdp text,actif text,totpsecret text,totpcode text, quota integer, musicviewmode text, imageviewmode text, videoviewmode text, documentviewmode text, pagewebviewmode text);
CREATE UNIQUE INDEX comptex ON compte (nom(100));
INSERT INTO compte values ('Admin','admin','oui','','',0,'noma','dated','noma','noma','noma');
CREATE TABLE entree (owner text, zone text, nom text, lien text, taille integer, id integer);
CREATE UNIQUE INDEX entreex ON entree (owner(100),zone(100),id);