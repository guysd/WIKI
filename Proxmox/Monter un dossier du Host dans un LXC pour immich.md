## 1. Créer le point de montage (Bind Mount)

Sur votre hôte Proxmox (en ligne de commande), exécutez ceci pour lier uniquement le dossier spécifique au LXC :

    bash

    # Remplacez ID_LXC par votre numéro de conteneur (ex: 100)
    pct set ID_LXC -mp0 /MON_DAS/immich_data,mp=/mnt/immich_photos
    Utilisez le code avec précaution.

    Le dossier /MON_DAS/immich_data sera désormais accessible dans /mnt/immich_photos à l'intérieur de votre LXC. Proxmox LXC Bind Mounts

## 2. Ajuster les permissions (Important)

    Si votre LXC est non-privilégié (configuration par défaut la plus sûre), vous devez donner les droits au mapping d'utilisateur du conteneur sur l'hôte Proxmox :
    
    bash

    # Sur l'hôte Proxmox
    chown -R 100000:100000 /MON_DAS/immich_data
    chmod -R 755 /MON_DAS/immich_data
    Utilisez le code avec précaution.

## 3. Configurer Immich dans le LXC

    Entrez dans votre conteneur LXC, allez dans votre dossier d'installation Immich et modifiez le fichier .env :

    env

    # Dans votre fichier .env
    UPLOAD_LOCATION=/mnt/immich_photos
    Utilisez le code avec précaution.

## 4. Appliquer les changements

    Redémarrez vos conteneurs Docker pour qu'ils basculent sur le nouveau stockage :

    bash
    docker compose up -d --force-recreate
    Utilisez le code avec précaution.

    Note : Si vous aviez déjà des photos dans l'ancien dossier upload d'Immich (sur le disque local du LXC), pensez à les déplacer vers /mnt/immich_photos avant de relancer Docker pour ne pas perdre l'accès à vos miniatures existantes.
