📁 Guide de Configuration : PostgREST + Swagger UI

Ce document récapitule la configuration nécessaire pour exposer une base de données PostgreSQL via une API REST automatique avec Swagger.

1. Configuration Docker (docker-compose.yml)
Assurez-vous que les ports internes et externes sont alignés.
yaml

services:
  postgrest:
    image: postgrest/postgrest:latest
    container_name: postgrest_api
    restart: unless-stopped
    ports:
      - "3005:3005" # Port accessible depuis votre navigateur
    environment:
      # Port interne sur lequel PostgREST écoute
      PGRST_SERVER_PORT: "3005"
      PGRST_SERVER_HOST: "0.0.0.0"
      # Autoriser l'accès depuis l'interface Swagger
      PGRST_SERVER_CORS_ALLOWED_ORIGINS: "http://192.168.2.109:8085"
    env_file:
      - .env

  swagger:
    image: swaggerapi/swagger-ui
    container_name: swagger_ui
    ports:
      - "8085:8080"
    environment:
      # URL de l'API PostgREST (doit être l'IP de la machine hôte)
      API_URL: http://192.168.2.109

Utilisez le code avec précaution.
2. Variables d'Environnement (.env)
📁 Guide de Configuration : PostgREST + Swagger UI

Ce document récapitule la configuration nécessaire pour exposer une base de données PostgreSQL via une API REST automatique avec Swagger.

1. Configuration Docker (docker-compose.yml)
Assurez-vous que les ports internes et externes sont alignés.
yaml

services:
  postgrest:
    image: postgrest/postgrest:latest
    container_name: postgrest_api
    restart: unless-stopped
    ports:
      - "3005:3005" # Port accessible depuis votre navigateur
    environment:
      # Port interne sur lequel PostgREST écoute
      PGRST_SERVER_PORT: "3005"
      PGRST_SERVER_HOST: "0.0.0.0"
      # Autoriser l'accès depuis l'interface Swagger
      PGRST_SERVER_CORS_ALLOWED_ORIGINS: "http://192.168.2.109:8085"
    env_file:
      - .env

  swagger:
    image: swaggerapi/swagger-ui
    container_name: swagger_ui
    ports:
      - "8085:8080"
    environment:
      # URL de l'API PostgREST (doit être l'IP de la machine hôte)
      API_URL: http://192.168.2.109

Utilisez le code avec précaution.
2. Variables d'Environnement (.env)
PGRST_DB_URI=postgres://postgrest:pwd@192.168.2.109:5432/stephnnails
PGRST_DB_SCHEMA=public
PGRST_DB_ANON_ROLE=anon
PGRST_SERVER_PORT=3005
# 1. Le NOM du rôle créé en SQL (celui des GRANT)
PGRST_DB_ANON_ROLE=web_anon 

# 2. Votre clé secrète pour les tokens (si vous en utilisez)
PGRST_JWT_SECRET=12ZZeeetosmxxyyghjkklkkss

Utilisez le code avec précaution.
3. Scripts SQL (Base de données)
Exécutez ce script dans votre client SQL (pgAdmin, DBeaver) pour créer la structure et accorder les droits.
sql

-- ==========================================
-- 1. GESTION DES RÔLES ET PERMISSIONS
-- ==========================================

-- Création du rôle anonyme (s'il n'existe pas)
DO $$ 
BEGIN
  IF NOT EXISTS (SELECT FROM pg_catalog.pg_roles WHERE rolname = 'web_anon') THEN
    CREATE ROLE web_anon nologin;
  END IF;
END $$;

-- IMPORTANT : Autoriser votre utilisateur principal (ex: postgres) 
-- à utiliser le rôle web_anon pour servir l'API
GRANT web_anon TO postgres;

-- Accès au schéma pour le rôle anonyme
GRANT USAGE ON SCHEMA public TO web_anon;


-- ==========================================
-- 2. CRÉATION DE LA TABLE 'CLIENT'
-- ==========================================

CREATE TABLE IF NOT EXISTS public.client (
    id SERIAL PRIMARY KEY,
    nom TEXT NOT NULL,
    prenom TEXT NOT NULL,
    email TEXT UNIQUE NOT NULL,
    telephone TEXT,
    date_creation TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP
);


-- ==========================================
-- 3. ATTRIBUTION DES DROITS SUR LA TABLE
-- ==========================================

-- Permettre la lecture (GET)
GRANT SELECT ON public.client TO web_anon;

-- Permettre l'écriture (POST)
GRANT INSERT ON public.client TO web_anon;

-- Permettre la modification/suppression (PATCH/DELETE)
GRANT UPDATE, DELETE ON public.client TO web_anon;

-- Permettre l'utilisation de l'ID auto-incrémenté (Indispensable pour l'INSERT)
GRANT USAGE, SELECT ON SEQUENCE client_id_seq TO web_anon;

Utilisez le code avec précaution.
4. Procédure de déploiement

    Appliquez le SQL ci-dessus sur votre base de données.
    Mettez à jour votre fichier docker-compose.yml et .env.
    Redémarrez les services pour forcer le rafraîchissement du cache :
    bash

    docker compose up -d --force-recreate

    Utilisez le code avec précaution.
    Vérifiez les logs pour confirmer que PostgREST voit la table :
    bash

    docker logs postgrest_api | grep "Schema cache loaded"

    Utilisez le code avec précaution.



Utilisez le code avec précaution.
3. Scripts SQL (Base de données)
Exécutez ce script dans votre client SQL (pgAdmin, DBeaver) pour créer la structure et accorder les droits.
sql

-- ==========================================
-- 1. GESTION DES RÔLES ET PERMISSIONS
-- ==========================================

-- Création du rôle anonyme (s'il n'existe pas)
DO $$ 
BEGIN
  IF NOT EXISTS (SELECT FROM pg_catalog.pg_roles WHERE rolname = 'web_anon') THEN
    CREATE ROLE web_anon nologin;
  END IF;
END $$;

-- IMPORTANT : Autoriser votre utilisateur principal (ex: postgres) 
-- à utiliser le rôle web_anon pour servir l'API
GRANT web_anon TO postgres;

-- Accès au schéma pour le rôle anonyme
GRANT USAGE ON SCHEMA public TO web_anon;


-- ==========================================
-- 2. CRÉATION DE LA TABLE 'CLIENT'
-- ==========================================

CREATE TABLE IF NOT EXISTS public.client (
    id SERIAL PRIMARY KEY,
    nom TEXT NOT NULL,
    prenom TEXT NOT NULL,
    email TEXT UNIQUE NOT NULL,
    telephone TEXT,
    date_creation TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP
);


-- ==========================================
-- 3. ATTRIBUTION DES DROITS SUR LA TABLE
-- ==========================================

-- Permettre la lecture (GET)
GRANT SELECT ON public.client TO web_anon;

-- Permettre l'écriture (POST)
GRANT INSERT ON public.client TO web_anon;

-- Permettre la modification/suppression (PATCH/DELETE)
GRANT UPDATE, DELETE ON public.client TO web_anon;

-- Permettre l'utilisation de l'ID auto-incrémenté (Indispensable pour l'INSERT)
GRANT USAGE, SELECT ON SEQUENCE client_id_seq TO web_anon;

Utilisez le code avec précaution.
4. Procédure de déploiement

    Appliquez le SQL ci-dessus sur votre base de données.
    Mettez à jour votre fichier docker-compose.yml et .env.
    Redémarrez les services pour forcer le rafraîchissement du cache :
    bash

    docker compose up -d --force-recreate

    Utilisez le code avec précaution.
    Vérifiez les logs pour confirmer que PostgREST voit la table :
    bash

    docker logs postgrest_api | grep "Schema cache loaded"

    Utilisez le code avec précaution.
 
