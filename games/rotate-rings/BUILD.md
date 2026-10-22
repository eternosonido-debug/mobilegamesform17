# Build e instalação — Rotate Rings

## Desktop/Linux

Dependências: CMake 3.16+, compilador C++11, SDL2 e Threads. SDL2 deve disponibilizar sua configuração CMake.

```sh
cmake -S games/rotate-rings -B build/rotate-rings -DCMAKE_PREFIX_PATH=/caminho/do/SDL2
cmake --build build/rotate-rings
ctest --test-dir build/rotate-rings --output-on-failure
./build/rotate-rings/rotate-rings
```

Também há Makefile na pasta do jogo. Para teste sem tela nem áudio físico:

```sh
SDL_VIDEODRIVER=dummy SDL_RENDER_DRIVER=software SDL_AUDIODRIVER=dummy ./build/rotate-rings/rotate-rings --smoke
```

A interface usa canvas lógico 240 × 136, escalado 2x sem suavização para 480 × 272. As 20 fases usam duas a cinco peças; as texturas dos anéis são geradas na inicialização e giradas pela SDL.

## Cross-compile confirmado para a unidade anterior

Foi usada a toolchain Zig e o runtime SDL 2.28.5 recuperado do M17 W12D RK3168. As bibliotecas do firmware não são distribuídas no repositório. Use os arquivos da sua unidade:

```sh
export M17_SDL_INCLUDE=/caminho/dos/headers/SDL2
export M17_SDL_LIBRARY=/caminho/do/runtime/libSDL2-2.0.so.0
export ZIG=/caminho/do/zig
sh games/rotate-rings/build-m17.sh
```

O script gera `build/m17/rotate-rings`, com alvo ARM gnueabi soft-float Cortex-A9. Não usar a biblioteca hard-float encontrada em outro diretório do cartão. O único componente compartilhado usado por este novo jogo é a leitura genérica de direcionais; regras, desenho, input de ações, áudio e save estão na pasta Rotate Rings.

## Instalação aditiva no cartão já identificado

1. Faça backup antes da cópia. O cartão deve estar desmontado do console e conectado ao leitor.
2. Copie o executável para `fmg-test/bin/rotate-rings-test` na partição de jogos já usada neste projeto.
3. Copie `integration/Rotate-Rings-Test.sh` para `.config/emuelec/ports/Rotate-Rings-Test.sh` na mesma partição.
4. Preserve `fmg-test/controls.sh`, todos os saves e os launchers antigos. Não altere es_systems ou arquivos de boot.
5. Ejete o cartão com segurança. Abra **Ports → Rotate-Rings-Test** no console.

O launcher usa `/sdcard/fmg-test`, o caminho de runtime `simulator/3168/lib_3168` e a pasta de saves que já funcionam na unidade previamente identificada. Esses caminhos não foram presumidos para outros firmwares.

## Validação pendente no aparelho

Esta entrega foi compilada e testada no computador; não foi executada no M17. Confirmar os dois manches físicos, mapeamento A/B/X/Y, legibilidade, som, retorno ao Ports e desempenho antes de declarar pronta para o console.
