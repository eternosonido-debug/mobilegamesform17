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

Interface em 480×272, com todos os passos na mesma tela e fonte pixel sem suavização. Áudio mono S16 a 22050 Hz, sete vozes de bateria, três melódicas e vozes separadas para prévia; blocos de 1024 amostras (cerca de 46 ms, latência real dependente do backend). Callback próprio mistura o banco preparado antes de abrir o dispositivo, sem alocação, trigonometria por amostra, desenho ou acesso a disco. Regras, síntese, interface e input estão separados. Apenas o helper genérico de direcionais de shared/fmg-runtime é reutilizado; os outros jogos não são dependências.

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

## Validação da revisão 0.6

Sete grupos específicos: regras/save; áudio dos quatro instrumentos, sete sons de bateria, sete notas diretas e prévia; controles com sete atalhos, A exclusivo e dois manches virtuais; áudio com interface bloqueada; variações/desfazer e migração dos saves antigos; janela completa com escolha de percussão, A, play, menu e save; todos os passos visíveis em 4/8/16/32.

Build local com AddressSanitizer/UndefinedBehaviorSanitizer. O repositório completo tem 23 grupos de testes. Screenshots da grade foram renderizados e inspecionados. A revisão anterior tinha seis linhas de edição; agora há quatro pistas, e o botão de nota escolhe o som da bateria sem subpistas ou menus. Menu Select e captura L1/R1/L2/R2 precisam ser confirmados na unidade.

O banco próprio ocupa aproximadamente 10,2 MB e é preparado antes de abrir o áudio. Há sete vozes independentes de percussão e três melódicas, além de vozes próprias de prévia. As notas diretas são Dó/Ré/Mi/Fá/Sol/Lá/Si; índices antigos de notas continuam sendo sintetizados para preservar os beats importados.

Pendente no M17: som, conforto/legibilidade em 32 passos, mapeamento físico, FPS, uso real de memória/CPU, ausência de cortes e retorno ao Ports. Não foi medida latência real nem realizado teste auditivo desta revisão no aparelho.

Save POCKETBEATS3 importa POCKETBEATS1/2 e preserva a escolha de nota de passos desligados. Preserve uma cópia do save antes da primeira execução; executáveis antigos não leem o formato novo. O arquivo pocket-beats-controls.cfg usa PBNOTES2 para quatro ombros e importa o PBPITCH1 anterior.
