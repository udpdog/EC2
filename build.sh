#!/usr/bin/env bash
set -euo pipefail

project_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
build_dir="${project_dir}/build/linux"

for command in cmake c++; do
    if ! command -v "${command}" >/dev/null 2>&1; then
        echo "Erreur : ${command} est introuvable." >&2
        echo "Installez CMake et un compilateur C++17 avec le gestionnaire de paquets de votre distribution." >&2
        exit 1
    fi
done

cmake -S "${project_dir}" -B "${build_dir}" -DCMAKE_BUILD_TYPE=Release
cmake --build "${build_dir}" --config Release --parallel
cp "${build_dir}/ec2" "${project_dir}/ec2"
chmod +x "${project_dir}/ec2"

echo "Termine : ${project_dir}/ec2"
