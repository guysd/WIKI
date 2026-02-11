# Guide Configuration VPN IPsec : Ubuntu vers Fortigate 60D

## 1. Installation des dépendances
Ouvrez un terminal et exécutez :
sudo apt update && sudo apt install strongswan charon-systemd libcharon-extra-plugins -y

## 2. Configuration du tunnel (/etc/ipsec.conf)
Modifiez le fichier avec : sudo nano /etc/ipsec.conf
Collez la configuration suivante :

config setup
    charondebug="ike 1, knl 1, cfg 1"

conn fortigate-60d
    keyexchange=ikev1
    authby=secret
    aggressive=yes
    left=%defaultroute
    leftauth=psk
    leftauth2=xauth
    xauth_identity="VOTRE_NOM_UTILISATEUR"
    right=IP_PUBLIQUE_FORTIGATE
    rightid=%any
    rightsubnet=192.168.1.0/24
    auto=add

## 3. Configuration des secrets (/etc/ipsec.secrets)
Modifiez le fichier avec : sudo nano /etc/ipsec.secrets
Ajoutez vos accès :

: PSK "VOTRE_CLE_PARTAGEE"
VOTRE_NOM_UTILISATEUR : XAUTH "VOTRE_MOT_DE_PASSE"

## 4. Commandes de gestion
- Démarrer : sudo ipsec restart && sudo ipsec up fortigate-60d
- Statut : sudo ipsec status
- Arrêter : sudo ipsec down fortigate-60d