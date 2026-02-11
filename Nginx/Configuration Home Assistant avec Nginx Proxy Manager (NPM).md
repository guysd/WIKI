## 1. Installer Nginx Proxy Manager (Docker)

  ### docker compose file

    version: '3.8'
    services:
      npm:
        image: jc21/nginx-proxy-manager:latest
        container_name: nginx-proxy-manager
        restart: unless-stopped
        ports:
          - "80:80"
          - "443:443"
          - "81:81"
        volumes:
          - ./data:/data
          - ./letsencrypt:/etc/letsencrypt

## 2. Autoriser le proxy dans Home Assistant

    Dans /config/configuration.yaml  :

    http:
      use_x_forwarded_for: true
      trusted_proxies:
        - 172.18.0.0/24
        - 192.168.1.XXX

    En cas d’erreur 400, l’IP exacte à ajouter apparaît dans les journaux de Home Assistant.

## 3. Configuration Nginx Proxy Manager (Port 81)

  ### Proxy Host

    Domain : ha.votre-domaine.fr

    Scheme : http

    Forward IP : IP locale de Home Assistant

    Port : 8123

    Websockets : ✅activé

  ### SSL

    Certificat Let's Encrypt

    Force SSL : ✅ activé

    Email valide requis

## 4. Redirection des ports (routeur)

    Port externe	Port interne	Destination
           80	        80	      Serveur NPM
           443	      443	      Serveur NPM

## 5. Résultat

    Accès externe sécurisé en HTTPS

    WebSockets fonctionnels

    Certificat SSL auto-renouvelé


## Liens 

  https://www.youtube.com/watch?v=bADNTSF4Fh4