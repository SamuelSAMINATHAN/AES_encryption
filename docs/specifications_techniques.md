# Spécifications Techniques Détaillées - AES-256-GCM Encryptor

## 1. Architecture du Système

### 1.1 Composants Principaux
```
┌─────────────────┐     ┌──────────────────┐     ┌─────────────────┐
│  Interface GUI  │     │  Moteur de       │     │  Interface CLI  │
│  (Qt/C++)      │────►│  Chiffrement     │◄────│  (C)           │
└─────────────────┘     │  (C/OpenSSL)     │     └─────────────────┘
                        └──────────────────┘

```

### 1.2 Modules
- **GUI Module** (GUI/main_gui.cpp)
  - Interface graphique Qt
  - Gestion des événements utilisateur
  - Validation des entrées
  
- **CLI Module** (CLI/main_cli.c)
  - Interface ligne de commande
  - Parsing des arguments
  - Gestion des erreurs
  
- **Core Crypto Module** (src/secure_aes_gcm.c)
  - Implémentation du chiffrement AES-256-GCM
  - Gestion des clés et des salts
  - Intégration OpenSSL

## 2. Spécifications Cryptographiques

### 2.1 Algorithmes
| Composant | Spécification |
|-----------|---------------|
| Chiffrement | AES-256-GCM |
| Mode d'opération | GCM (Galois/Counter Mode) |
| Taille de clé | 256 bits |
| PBKDF2 | HMAC-SHA256, 100 000 itérations |
| Salt | 16 octets aléatoires |
| Nonce | 12 octets aléatoires |
| Tag d'authentification | 16 octets |

### 2.2 Format des Données
```
┌──────────┬──────────┬────────────────┬──────────┐
│   Salt   │  Nonce   │ Données        │   Tag    │
│ 16 bytes │ 12 bytes │ chiffrées      │ 16 bytes │
└──────────┴──────────┴────────────────┴──────────┘
```

## 3. Interfaces de Programmation (API)

### 3.1 Interface C (secure_aes_gcm.h)
```c
// Principales fonctions exposées
int encrypt_file(const char* input_file, const char* output_file, const char* password);
int decrypt_file(const char* input_file, const char* output_file, const char* password);
int generate_key(const char* password, unsigned char* key, unsigned char* salt);
```

### 3.2 Gestion des Erreurs
| Code | Description |
|------|-------------|
| 0 | Succès |
| -1 | Erreur fichier |
| -2 | Erreur mémoire |
| -3 | Erreur cryptographique |
| -4 | Données corrompues |

## 4. Contraintes Techniques

### 4.1 Performance
- Temps de dérivation de clé : < 1 seconde sur CPU moderne
- Vitesse de chiffrement : > 100 MB/s
- Utilisation mémoire maximale : 512 MB

### 4.2 Compatibilité
- **OS** : macOS 10.15+, Linux, Windows 10+
- **Compilateurs** : GCC 8+, Clang 10+, MSVC 2019+
- **Dépendances** :
  - Qt 5.15+ (GUI)
  - OpenSSL 1.1.1+
  - CMake 3.15+

## 5. Sécurité

### 5.1 Mesures de Protection
- Effacement sécurisé des clés en mémoire
- Protection contre les attaques par force brute via PBKDF2
- Vérification d'intégrité via GCM
- Nonce unique par fichier

### 5.2 Limitations Connues
- Pas de reprise sur erreur pendant le chiffrement
- Taille maximale de fichier limitée par la RAM
- Pas de compression des données