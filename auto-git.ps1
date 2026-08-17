$delay = 3

# On transforme toujours le résultat Git en une seule chaîne
function Get-GitState {
    return ((git status --porcelain 2>$null) -join "`n").Trim()
}

$lastState = Get-GitState
$lastChange = Get-Date

Write-Host "Auto Git actif - attente des modifications..."

# S'il y a déjà des modifications au démarrage
if ($lastState) {
    Write-Host "Modifications deja presentes - commit dans $delay secondes..."
}

while ($true) {

    $currentState = Get-GitState

    # Un vrai changement vient d'apparaitre
    if ($currentState -ne $lastState) {

        $lastState = $currentState
        $lastChange = Get-Date

        if ($currentState) {
            Write-Host "Modification detectee - commit dans $delay secondes..."
        }
    }

    # Il existe des modifications
    if ($currentState) {

        $elapsed = ((Get-Date) - $lastChange).TotalSeconds

        if ($elapsed -ge $delay) {

            Write-Host "Commit en cours..."

            git add .

            # Vérifie qu'il y a réellement quelque chose à commit
            git diff --cached --quiet

            if ($LASTEXITCODE -ne 0) {

                $date = Get-Date -Format "yyyy-MM-dd HH:mm:ss"

                git commit -m "Auto save - $date"

                if ($LASTEXITCODE -eq 0) {
                    Write-Host "Push vers GitHub..."

                    git push

                    if ($LASTEXITCODE -eq 0) {
                        Write-Host "Push termine."
                    }
                    else {
                        Write-Host "Erreur pendant le push."
                    }
                }
            }

            # Recalcule l'état après le commit
            $lastState = Get-GitState
            $lastChange = Get-Date

            Write-Host "Surveillance reprise..."
        }
    }

    Start-Sleep -Seconds 2
}