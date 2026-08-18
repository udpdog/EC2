<div align="center">

<img src=".github/banner.png" alt="Banner EC2" width="700">

[![Latest release](https://img.shields.io/github/v/release/udpdog/EC2?style=for-the-badge&label=Version)](https://github.com/udpdog/EC2/releases/latest)
[![Build and release](https://img.shields.io/github/actions/workflow/status/udpdog/EC2/release.yml?style=for-the-badge&label=Build)](https://github.com/udpdog/EC2/actions/workflows/release.yml)
[![Windows x64](https://img.shields.io/badge/Windows-x64-0078D4?style=for-the-badge&logo=windows)](#installation)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-00599C?style=for-the-badge&logo=cplusplus)](#compilation)

EC2 convertit des images, fichiers RAW et documents vers des formats courants
avec une syntaxe courte et des options avancées adaptées au format de sortie.

[Installation](#installation) · [Utilisation](#utilisation) · [Formats](#formats) · [Options avancées](#options-avancées) · [Compilation](#compilation)

</div>

---

## Sommaire

- [Fonctionnalités](#fonctionnalités)
- [Installation](#installation)
- [Utilisation](#utilisation)
  - [Options générales](#options-générales)
  - [Fichier de sortie](#fichier-de-sortie)
- [Formats](#formats)
  - [Formats d’entrée](#formats-dentrée)
  - [Formats de sortie](#formats-de-sortie)
- [Options avancées](#options-avancées)
  - [Redimensionnement](#redimensionnement)
  - [Qualité et compression](#qualité-et-compression)
- [Exemples](#exemples)
- [Compilation](#compilation)
- [Dépendances](#dépendances)
- [Structure du projet](#structure-du-projet)
- [Limites connues](#limites-connues)
- [Publication d’une version](#publication-dune-version)

## Fonctionnalités

- Conversion depuis de nombreux formats classiques, RAW et documentaires.
- Douze formats de sortie : AVIF, BMP, EPS, GIF, ICO, JPG, PDF, PNG, PS,
  PSD, TIFF et WEBP.
- Redimensionnement proportionnel, recadrage ou mise à l’échelle forcée.
- Orientation automatique à partir des métadonnées EXIF.
- Conservation ou suppression des métadonnées.
- Réglage de la qualité pour JPG, PNG et WEBP.
- Gestion sûre des fichiers existants avec confirmation explicite via
  `--force`.
- Installation automatique d’ImageMagick lorsqu’il est absent.
- Chemins Windows Unicode et chemins contenant des espaces.

## Installation

### Exécutable Windows

Téléchargez [`ec2.exe`](https://github.com/udpdog/EC2/releases/latest/download/ec2.exe)
depuis la [dernière version publiée](https://github.com/udpdog/EC2/releases/latest),
puis placez-le dans le dossier de votre choix.

Dans PowerShell ou l’invite de commandes :

```powershell
.\ec2.exe --help
```

EC2 utilise ImageMagick comme moteur de conversion. S’il n’est pas présent,
EC2 tente de l’installer automatiquement avec `winget` lors de la première
conversion. Une connexion Internet peut donc être nécessaire au premier
lancement.

> [!TIP]
> Ajoutez le dossier contenant `ec2.exe` à votre variable `PATH` pour pouvoir
> exécuter `ec2` depuis n’importe quel répertoire.

## Utilisation

```text
ec2.exe <image> -format <format> [-o <sortie>] [--force] [options]
```

Conversion minimale :

```powershell
.\ec2.exe "image.png" -format jpg
```

### Options générales

| Option | Description |
|---|---|
| `-format FORMAT`, `-f FORMAT` | Définit le format de sortie. |
| `-o CHEMIN`, `--output CHEMIN` | Définit le fichier de sortie. |
| `--force` | Autorise l’écrasement d’un fichier existant. |
| `--formats` | Affiche les formats de sortie disponibles. |
| `-h`, `--help` | Affiche l’aide générale. |
| `-h -f FORMAT` | Affiche les options avancées d’un format. |
| `--help FORMAT` | Variante courte de l’aide par format. |

### Fichier de sortie

Sans `-o`, le résultat est créé à côté de l’image d’origine :

```powershell
.\ec2.exe "C:\Photos\vacances.png" -format webp
# Résultat : C:\Photos\vacances.webp
```

Si l’entrée possède déjà l’extension demandée, EC2 ajoute `_converted` pour ne
pas remplacer l’original :

```text
photo.jpg -> photo_converted.jpg
```

EC2 refuse d’écraser un fichier existant par défaut. Utilisez `--force` pour
l’autoriser explicitement :

```powershell
.\ec2.exe image.png -format jpg --force
```

## Formats

### Formats d’entrée

| Catégorie | Formats acceptés |
|---|---|
| Images courantes | AVIF, BMP, GIF, HEIC, HEIF, ICO, JFIF, JPEG, JPG, PNG, PPM, TGA, TIF, TIFF, WEBP |
| Images RAW | 3FR, ARW, CR2, CR3, CRW, DCR, DNG, ERF, MOS, MRW, NEF, ORF, PEF, RAF, RAW, RW2, X3F |
| Documents et création | EPS, ICNS, ODG, PS, PSB, PSD, PUB, XCF, XPS |

La présence d’un format dans cette liste signifie qu’EC2 l’accepte comme
entrée. Le décodage réel dépend des codecs et délégués disponibles dans
l’installation d’ImageMagick.

### Formats de sortie

```text
AVIF  BMP  EPS  GIF  ICO  JPG
PDF   PNG  PS   PSD  TIFF WEBP
```

Pour obtenir cette liste depuis le programme :

```powershell
.\ec2.exe --formats
```

## Options avancées

Les détails restent hors de l’aide générale pour conserver une sortie lisible.
Affichez l’aide du format voulu :

```powershell
.\ec2.exe -h -f jpg
.\ec2.exe --help png
.\ec2.exe --help ico
```

Tous les formats de sortie acceptent les options communes suivantes :

| Option | Valeur | Description |
|---|---:|---|
| `--width N` | `1..100000` | Largeur maximale ou forcée en pixels. |
| `--height N` | `1..100000` | Hauteur maximale ou forcée en pixels. |
| `--fit max` | Par défaut | Conserve les proportions et n’agrandit pas les petites images. |
| `--fit crop` | Largeur + hauteur requises | Remplit la zone puis recadre au centre. |
| `--fit scale` | Largeur + hauteur requises | Force exactement la largeur et la hauteur. |
| `--strip yes\|no` | `no` | Supprime ou conserve les métadonnées. |
| `--auto-orient yes\|no` | `yes` | Applique l’orientation EXIF avant le redimensionnement. |

### Redimensionnement

#### Ajustement proportionnel

```powershell
.\ec2.exe photo.jpg -format webp --width 1920 --height 1080 --fit max
```

L’image tient dans une zone de 1920 × 1080 sans être déformée ni agrandie si
elle est déjà plus petite.

#### Remplissage et recadrage

```powershell
.\ec2.exe photo.jpg -format png --width 800 --height 800 --fit crop
```

L’image remplit entièrement la zone de 800 × 800, puis l’excédent est retiré au
centre.

#### Dimensions forcées

```powershell
.\ec2.exe photo.jpg -format bmp --width 640 --height 480 --fit scale
```

Le résultat mesure exactement 640 × 480, même si cela modifie les proportions.

### Qualité et compression

`--quality` accepte une valeur de `1` à `100` pour les formats suivants :

| Format | Comportement | Valeur par défaut |
|---|---|---:|
| JPG | Qualité JPEG ; une valeur élevée réduit la compression. | Automatique |
| PNG | Niveau de compression zlib et filtre PNG. | `75` |
| WEBP | Qualité de compression WEBP. | Automatique |

```powershell
.\ec2.exe image.png -format jpg --quality 90
.\ec2.exe image.jpg -format webp --quality 80
.\ec2.exe image.bmp -format png --quality 75
```

### Valeurs particulières

Le format ICO utilise par défaut une zone de 32 × 32 pixels :

```powershell
.\ec2.exe logo.png -format ico
```

Avec `fit=max`, les proportions sont conservées. Pour forcer un carré exact :

```powershell
.\ec2.exe logo.png -format ico --width 32 --height 32 --fit scale
```

## Exemples

### RAW vers WEBP

```powershell
.\ec2.exe "C:\Photos\photo.CR2" -format webp --quality 85
```

### PNG vers PDF

```powershell
.\ec2.exe image.png -format pdf --width 1920 --height 1080 --fit max
```

### Image carrée pour un avatar

```powershell
.\ec2.exe portrait.jpg -format avif --width 512 --height 512 --fit crop --strip yes
```

### Sortie personnalisée

```powershell
.\ec2.exe source.psd -format png -o "C:\Exports\resultat.png" --force
```

### Corriger l’orientation sans supprimer les métadonnées

```powershell
.\ec2.exe photo.jpg -format tiff --auto-orient yes --strip no
```

## Compilation

EC2 cible Windows x64 et utilise C++17 avec CMake.

Clonez le dépôt puis lancez :

```powershell
git clone https://github.com/udpdog/EC2.git
cd EC2
.\build.cmd
```

`build.cmd` contourne la stratégie d’exécution PowerShell uniquement pour le
processus de compilation. Il ne modifie pas la politique globale du système.

Le script installe automatiquement avec `winget`, si nécessaire :

- ImageMagick ;
- CMake ;
- Visual Studio Build Tools 2022 avec les outils C++.

À la fin de la compilation, l’exécutable est copié vers :

```text
EC2\ec2.exe
```

### Compilation manuelle

Si les outils sont déjà installés :

```powershell
cmake -S . -B build
cmake --build build --config Release
Copy-Item .\build\Release\ec2.exe .\ec2.exe
```

## Dépendances

| Dépendance | Utilisation | Installation automatique |
|---|---|:---:|
| Windows 10/11 x64 | Plateforme cible | — |
| ImageMagick 7 | Lecture, transformation et encodage | Oui |
| `winget` | Installation des composants manquants | Requis si un composant manque |
| CMake | Génération du projet | Oui, pour compiler |
| Visual Studio Build Tools | Compilateur C++ | Oui, pour compiler |

EC2 exécute `magick.exe` directement avec `CreateProcessW` et ne construit pas
une commande passée à un interpréteur shell.

## Structure du projet

```text
EC2/
├── .github/workflows/   Workflows GitHub Actions
├── src/
│   ├── main.cpp         Point d’entrée et orchestration
│   ├── cli.*            Analyse de la ligne de commande
│   ├── formats.*        Formats d’entrée et de sortie
│   ├── conversion_options.*
│   │                    Profils et validation des options avancées
│   └── imagemagick.*    Détection et exécution d’ImageMagick
├── build.cmd            Lanceur de compilation Windows
├── build.ps1            Installation et compilation automatisées
└── CMakeLists.txt        Configuration CMake
```

## Limites connues

- Les formats RAW sont principalement conçus pour être lus, pas générés.
- La disponibilité de certains formats d’entrée dépend des délégués compilés
  avec ImageMagick.
- Une image simple convertie en PSD produit généralement un document aplati,
  sans calques Photoshop complexes.
- Les formats documentaires tels que XPS, PUB ou ODG peuvent nécessiter des
  composants supplémentaires pour être décodés.
- `fit=scale` peut déformer l’image puisque les proportions ne sont pas
  conservées.

## Publication d’une version

Le workflow GitHub Actions crée automatiquement une version lorsque vous
envoyez un tag commençant par `v` :

```powershell
git tag v1.0.0
git push origin v1.0.0
```

Le workflow compile `ec2.exe`, vérifie sa présence, crée la GitHub Release et y
joint l’exécutable Windows x64.

---

<div align="center">

Développé en C++17 pour Windows.

</div>
