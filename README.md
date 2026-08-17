# EC2

Convertisseur d'images en ligne de commande pour Windows, écrit en C++17.
Il utilise ImageMagick pour prendre en charge un grand nombre de codecs.

## Prérequis

Installer Visual Studio Build Tools (charge de travail **Développement Desktop
en C++**) et CMake. ImageMagick est installé automatiquement avec `winget` par
le script de compilation ou par `ec2.exe` lors de sa première utilisation.

## Compilation

Dans un terminal PowerShell ouvert dans ce dossier :

```powershell
.\build.ps1
```

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

La lecture et l'écriture réelles d'un format dépendent des codecs et délégués
présents dans l'installation d'ImageMagick. Beaucoup de formats RAW sont faits
pour être lus, pas générés. Certains formats documentaires nécessitent aussi
Ghostscript ou LibreOffice.
