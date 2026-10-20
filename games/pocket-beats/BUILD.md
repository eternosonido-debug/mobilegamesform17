# Compilar e instalar Pocket Beats

## Computador

C++11, CMake 3.16+, SDL2 com configuração CMake:

```sh
cmake -S games/pocket-beats -B build/pocket-beats -DCMAKE_PREFIX_PATH=/caminho/do/SDL2
cmake --build build/pocket-beats
ctest --test-dir build/pocket-beats --output-on-failure
./build/pocket-beats/pocket-beats
```

Makefile equivalente na pasta do jogo. Teste sem janela/áudio físicos:

```sh
SDL_VIDEODRIVER=dummy SDL_RENDER_DRIVER=software SDL_AUDIODRIVER=dummy ./build/pocket-beats/pocket-beats --smoke --screenshot preview.bmp
```

Canvas lógico 240×136, saída inicial 480×272, escala sem suavização. Áudio mono S16 a 22050 Hz, quatro vozes sintetizadas; blocos de 1024 amostras (cerca de 46 ms, latência real dependente do backend). Callback próprio mistura o banco preparado antes de abrir o dispositivo, sem alocação, trigonometria por amostra, desenho ou acesso a disco. Regras, síntese, interface e input estão separados. Apenas o helper genérico de direcionais de shared/fmg-runtime é reutilizado; os outros jogos não são dependências.

## ARM do M17 anteriormente identificado

Toolchain e runtime já recuperados da unidade W12D RK3168, SDL 2.28.5 soft-float. Bibliotecas do firmware não são redistribuídas:

```sh
M17_SDL_INCLUDE=/caminho/headers/SDL2 \
M17_SDL_LIBRARY=/caminho/runtime/libSDL2-2.0.so.0 \
ZIG=/caminho/zig sh games/pocket-beats/build-m17.sh
```

Saída `build/m17/pocket-beats`, alvo ARM gnueabi soft-float Cortex-A9. O script e a inspeção de imports foram executados nesta entrega. Isto não confirma execução no hardware.

## Instalação aditiva via SD

Somente no cartão/firmware que já usa `/sdcard/fmg-test` e `/sdcard/simulator/3168/lib_3168`:

1. Faça backup e conecte o cartão ao leitor.
2. Copie o binário para `fmg-test/bin/pocket-beats-test` na partição de jogos.
3. Copie `integration/Pocket-Beats-Test.sh` para `.config/emuelec/ports/Pocket-Beats-Test.sh`.
4. Preserve controls.sh, bibliotecas, launchers, boot e saves existentes. Não altere es_systems.
5. Ejete com segurança e abra **Ports → Pocket-Beats-Test**.

Select salva e encerra para o menu original. Save próprio: `fmg-test/saves/pocket-beats.sav`; log `fmg-test/pocket-beats-teste.log`. No computador usa SDL_GetPrefPath, ou a pasta definida por FMG_SAVE_DIR. Gravação temporária e renomeação protegem contra arquivos parciais durante a gravação normal; não há garantia contra falha física do cartão.

## Validação

Quatro grupos específicos: regras/save, áudio dos quatro instrumentos e controles SDL com ambos os manches virtuais e reprodução independente da interface bloqueada. Build local também verificado com AddressSanitizer/UndefinedBehaviorSanitizer. Os 20 grupos do repositório passaram. A revisão 0.3 acrescenta ao teste de controles alteração de nota por gatilhos, captura digital, persistência e cancelamento do mapeamento. Screenshot de smoke renderizado e inspecionado; demonstração sonora gerada diretamente pelo sintetizador.

Pendente no M17: qualidade auditiva, mapeamento físico A/B/X/Y e ambos os manches, fluidez/FPS, ausência de cortes de áudio e retorno ao Ports. Não foi medida latência real nem realizado teste auditivo no console.
