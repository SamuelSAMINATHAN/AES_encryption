# AES-256-GCM Encryptor

## Description

Solution de chiffrement sécurisée utilisant l'algorithme AES-256-GCM avec dérivation de clé PBKDF2. Cette application est disponible en version GUI (interface graphique) et CLI (ligne de commande).

### Caractéristiques principales

- Chiffrement AES-256-GCM robuste
- Dérivation de clé via PBKDF2 (HMAC-SHA256)
- Interface graphique intuitive
- Interface en ligne de commande pour l'automatisation
- Protection contre les attaques par force brute
- Vérification d'intégrité des données

## Spécifications techniques

### Architecture de sécurité

#### Algorithmes cryptographiques
- **Chiffrement** : AES-256-GCM (Galois/Counter Mode)
- **Dérivation de clé** : PBKDF2 avec HMAC-SHA256
- **Longueur de clé** : 256 bits
- **Itérations PBKDF2** : 100 000 (configurable)

#### Format du fichier chiffré
```
[16 octets SALT | 12 octets NONCE | ...données chiffrées... | 16 octets TAG]
```

| Composant | Taille | Description |
|-----------|---------|-------------|
| Salt | 16 octets | Valeur aléatoire pour la dérivation de clé |
| Nonce | 12 octets | Vecteur d'initialisation unique pour GCM |
| Tag | 16 octets | Tag d'authentification GCM |

### Mesures de sécurité

1. **Protection contre les attaques par force brute**
   - Salt aléatoire de 16 octets
   - 100 000 itérations PBKDF2 (paramétrable)
   - Dérivation de clé robuste

2. **Sécurité des données**
   - Authentification des données via GCM
   - Vérification d'intégrité par tag
   - Effacement sécurisé des clés en mémoire

3. **Bonnes pratiques**
   - Nonce unique pour chaque chiffrement
   - Validation cryptographique des données
   - Gestion sécurisée de la mémoire

## Installation

### Prérequis
- Qt Framework
- Compilateur C++ compatible (GCC, Clang)
- OpenSSL (pour les primitives cryptographiques)

### Structure du projet
```
MonProjectAES/
├── include/
│   └── secure_aes_gcm.h    # En-têtes des fonctions cryptographiques
├── src/
│   └── secure_aes_gcm.c    # Implémentation du chiffrement
├── CLI/
│   ├── cli.pro            # Fichier projet Qt CLI
│   └── main_cli.c         # Point d'entrée CLI
└── GUI/
    ├── gui.pro            # Fichier projet Qt GUI
    └── main_gui.cpp       # Point d'entrée GUI
```

### Compilation

#### Version CLI
```bash
cd CLI
qmake cli.pro
make
```

#### Version GUI
```bash
cd GUI
qmake gui.pro
make
```

## Utilisation

### Interface en ligne de commande
```bash
# Chiffrement
./encryptor-cli encrypt fichier.txt fichier_chiffre.aes "PassphraseSécurisée"

# Déchiffrement
./encryptor-cli decrypt fichier_chiffre.aes fichier_dechiffre.txt "PassphraseSécurisée"
```

### Interface graphique
1. Lancer l'application `encryptor-gui`
2. Sélectionner le mode (chiffrement/déchiffrement)
3. Choisir le fichier source
4. Définir le fichier de destination
5. Entrer la passphrase
6. Valider l'opération

## Bonnes pratiques de sécurité

### Recommandations pour les utilisateurs
- Utiliser des passphrases fortes (>12 caractères)
- Ne pas réutiliser les passphrases
- Sauvegarder les passphrases de manière sécurisée
- Vérifier l'intégrité des fichiers après déchiffrement

### Limitations connues
- Les performances peuvent varier selon le nombre d'itérations PBKDF2
- La taille des fichiers est limitée par la mémoire disponible

## Maintenance et support

### Journal des versions
- v1.0.0 : Version initiale avec support CLI et GUI

### Évolutions futures envisagées
- Support d'Argon2 pour la dérivation de clé
- Mode de traitement par blocs pour les gros fichiers
- Interface en ligne de commande améliorée
- Support multi-plateforme testé

### Contact
Pour toute question technique ou rapport de bug, veuillez créer une issue dans le dépôt du projet.

## Licence
Ce projet est distribué sous licence [À DÉFINIR]. Voir le fichier LICENSE pour plus de détails.# AES_encryption
