# Ambiente M17 recuperado

Unidade identificada no trabalho anterior: M17 W12D, Rockchip RK3168, Linux armv7l, runtime de simulator/3168. SDL 2.28.5. ABI efetivamente usada: ARM gnueabi soft-float, interpretador /lib/ld-linux.so.3. A biblioteca hard-float encontrada em outro local do cartão não é o runtime usado pelos Ports que funcionaram.

A compilação anterior utilizou Zig, alvo arm-linux-gnueabi.2.29 e CPU cortex_a9. Nenhuma biblioteca extraída do aparelho é publicada. O usuário precisa fornecer os headers SDL e a biblioteca exata do seu runtime.

```sh
export M17_SDL_INCLUDE=/caminho/para/headers/SDL2
export M17_SDL_LIBRARY=/caminho/para/libSDL2-2.0.so.0
export ZIG=/caminho/para/zig
sh tools/build-m17.sh
```

O script gera um executável por jogo em build/m17. Esses nomes são entradas separadas para o mesmo motor preservado, iniciando diretamente o jogo escolhido. Não copia nada para o cartão.

A instalação no SD permanece pelo Ports que já funciona: /sdcard/fmg-test/bin, assets em /sdcard/fmg-test/assets e saves separados em /sdcard/fmg-test/saves. Os launchers e o mapa de botões recuperados estão em shared/fmg-runtime/integration/ports-test. Não alterar os arquivos originais do menu ou do boot.

Para jogos novos sem instalação individual, criar e verificar seu launcher antes de escrever no cartão. Nunca tratar uma compilação ou análise estática como teste de hardware.
