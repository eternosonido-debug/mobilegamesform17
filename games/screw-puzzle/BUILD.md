# Build e instalação — Screw Puzzle

## Desktop/Linux

Dependências: CMake 3.16+, compilador C++11, SDL2 e Threads. SDL2 deve disponibilizar sua configuração CMake.

```sh
cmake -S games/screw-puzzle -B build/screw-puzzle -DCMAKE_PREFIX_PATH=/caminho/do/SDL2
cmake --build build/screw-puzzle
ctest --test-dir build/screw-puzzle --output-on-failure
./build/screw-puzzle/screw-puzzle
```

Também há Makefile na pasta do jogo. Para teste sem tela nem áudio físico:

```sh
SDL_VIDEODRIVER=dummy SDL_RENDER_DRIVER=software SDL_AUDIODRIVER=dummy ./build/screw-puzzle/screw-puzzle --smoke
```

A interface usa canvas lógico 240 × 136, escalado 2x sem suavização para 480 × 272. O tabuleiro usa 16 posições fixas; somente os furos definidos pela fase podem receber parafusos.

## Cross-compile confirmado para a unidade anterior

Foi usada a toolchain Zig e o runtime SDL 2.28.5 recuperado do M17 W12D RK3168. As bibliotecas do firmware não são distribuídas no repositório. Use os arquivos da sua unidade:

```sh
export M17_SDL_INCLUDE=/caminho/dos/headers/SDL2
export M17_SDL_LIBRARY=/caminho/do/runtime/libSDL2-2.0.so.0
export ZIG=/caminho/do/zig
sh games/screw-puzzle/build-m17.sh
```

O script gera `build/m17/screw-puzzle`, com alvo ARM gnueabi soft-float Cortex-A9. Não usar a biblioteca hard-float encontrada em outro diretório do cartão. O único componente compartilhado usado por este novo jogo é a leitura genérica de direcionais; regras, desenho, input de ações, áudio e save estão na pasta Screw Puzzle.

## Instalação aditiva no cartão já identificado

1. Faça backup antes da cópia. O cartão deve estar desmontado do console e conectado ao leitor.
2. Copie o executável para `fmg-test/bin/screw-puzzle-test` na partição de jogos já usada neste projeto.
3. Copie `integration/Screw-Puzzle-Test.sh` para `.config/emuelec/ports/Screw-Puzzle-Test.sh` na mesma partição.
4. Preserve `fmg-test/controls.sh`, todos os saves e os launchers antigos. Não altere es_systems ou arquivos de boot.
5. Ejete o cartão com segurança. Abra **Ports → Screw-Puzzle-Test** no console.

O launcher usa `/sdcard/fmg-test`, o caminho de runtime `simulator/3168/lib_3168` e a pasta de saves que já funcionam na unidade previamente identificada. Esses caminhos não foram presumidos para outros firmwares.

## Validação pendente no aparelho

Esta entrega foi compilada e testada no computador; não foi executada no M17. Confirmar os dois manches físicos, mapeamento A/B/X/Y, legibilidade, som, retorno ao Ports e desempenho antes de declarar pronta para o console.
