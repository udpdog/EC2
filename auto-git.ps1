$delay = 30
$lastChange = Get-Date
$lastState = ""

Write-Host "Auto Git actif - attente des modifications..."

while ($true) {

    $currentState = git status --porcelain

    if ($currentState -ne $lastState) {
        if ($currentState) {
            $lastChange = Get-Date
            Write-Host "Modification detectee - commit dans $delay secondes..."
        }

        $lastState = $currentState
    }

    if ($currentState) {
        $elapsed = ((Get-Date) - $lastChange).TotalSeconds

        if ($elapsed -ge $delay) {
            Write-Host "Commit en cours..."

            git add .

            $date = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
            git commit -m "Auto save - $date"

            if ($LASTEXITCODE -eq 0) {
                Write-Host "Push vers GitHub..."
                git push
            }

            $lastState = git status --porcelain
            $lastChange = Get-Date

            Write-Host "Termine - surveillance reprise."
        }
    }

    Start-Sleep -Seconds 2
}