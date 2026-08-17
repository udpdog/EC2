# EC2

Convertisseur d'images en ligne de commande pour Windows, écrit en C++17.
Il utilise ImageMagick pour prendre en charge un grand nombre de codecs.

## Prérequis

Les dépendances manquantes sont installées automatiquement avec `winget` :
ImageMagick, CMake et Visual Studio Build Tools avec les outils C++. La première
compilation peut donc prendre plusieurs minutes et demander une autorisation
Windows.

## Compilation

Dans un terminal PowerShell ou une invite de commandes ouverte dans ce dossier :

```powershell
.\build.cmd
```

`build.cmd` lance le script avec une exception de politique limitée à ce seul
processus. Il ne modifie pas la stratégie d'exécution PowerShell du système.

## Utilisation

```powershell
.\ec2.exe "image.png" -format jpg
.\ec2.exe "C:\Photos\photo.CR2" -format webp
.\ec2.exe image.png -format avif -o "C:\Exports\photo.avif"
```

Sans `-o`, le résultat est créé à côté de l'original. Le programme refuse
d'écraser un fichier existant, sauf avec `--force`.

```powershell
.\ec2.exe image.png -format jpg --force
.\ec2.exe --formats
.\ec2.exe --help
```

Formats de sortie disponibles : `AVIF`, `BMP`, `EPS`, `GIF`, `ICO`, `JPG`,
`ODD`, `PNG`, `PS`, `PSD`, `TIFF` et `WEBP`. La liste plus large des formats
RAW, photo et documentaires est réservée aux fichiers d'entrée.

La lecture et l'écriture réelles d'un format dépendent des codecs et délégués
présents dans l'installation d'ImageMagick. Beaucoup de formats RAW sont faits
pour être lus, pas générés. Certains formats documentaires nécessitent aussi
Ghostscript ou LibreOffice.
