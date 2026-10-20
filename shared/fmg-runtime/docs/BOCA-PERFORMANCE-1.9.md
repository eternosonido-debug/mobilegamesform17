# Boca 1.9 — desempenho

O chão deixa de ser recalculado ponto a ponto a cada quadro. Uma imagem da ilha é gerada previamente e apenas sua região visível é desenhada. O novo arquivo `assets/boca-ground.bmp` deve acompanhar o binário. Se ausente, mantém-se o desenho procedural anterior. A carga ocorre antes do início do movimento.

A espera fixa de 16 ms após renderizar foi substituída pela espera do tempo restante do quadro, evitando somar uma espera completa à sincronização da tela.

## Validação

Cinco grupos de testes passaram com a compilação local instrumentada: regras/saves, Juices Sort, controles/saves do Boca, cidade/inimigos/progressão, transparência e carregamento/recarregamento dos assets. O teste dos assets inclui o novo chão em três níveis de zoom e três posições de câmera.

Na medição local com SDL software, 200 quadros após cinco de aquecimento, o tempo médio inicial passou de 0,862 ms para 0,376 ms. Com raio 75 passou de 1,070 para 0,366 ms; próximo da costa de 0,523 para 0,388 ms. Estes números medem desenho no computador e não representam FPS no M17.

Compilação ARM gnueabi soft-float Cortex-A9: 540 imports verificados contra o runtime SDL 2.28.5 e bibliotecas do aparelho, sem falhas estáticas. Nenhum teste desta versão no hardware foi realizado.

O cache ocupa aproximadamente 7,7 MB de pixels, além do atlas existente. Não altera os saves ou a jogabilidade. Regere-o pelo alvo CMake `fmg-bake-ground`, passando o caminho de saída ao executável.
