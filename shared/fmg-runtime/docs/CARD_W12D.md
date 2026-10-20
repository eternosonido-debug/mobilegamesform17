# Build preparado para o cartão inspecionado em 19/10/2026

## Diagnóstico de inicialização confirmado em 19/10/2026

Após as duas tentativas de categoria sem sucesso, um registro coletado no próprio
aparelho confirmou a execução de `simulator/3168/em_ui.sh`, kernel Linux 3.0.36+,
ARMv7, modelo M17 e placa W12D. Isso confirma a rota de inicialização; os jogos
ainda não tiveram execução confirmada. O sistema é Buildroot com frontend
customizado, apesar dos arquivos e da apresentação EmuELEC no cartão.

O menu ativo lê a lista codificada em `simulator/3168/.emulationstation/es_systems.cfg`
e produz `/tmp/es_systems.cfg`. A rotina `SystemData::getConfigPath` verifica antes
`/storage/.emulationstation/es_systems_custom.cfg`; o próprio registro confirmou
`/storage -> sdcard -> mnt/sdcard`. A instalação atual usa esse override, preserva
os 87 sistemas da lista ativa e acrescenta uma categoria (88 no total). A lista
codificada, os menus executáveis e o firmware não foram alterados.

As alterações anteriores nas duas listas plaintext da raiz foram desfeitas. Os
ganchos temporários nos scripts de boot também foram removidos, e seus conteúdos
originais foram conferidos. A configuração personalizada ainda aguarda reteste no
console; não é uma instalação universal para todas as revisões de M17.


**Atualização após tentativa no console:** a categoria da instalação inicial não
apareceu. O arquivo adicional sozinho não foi suficiente nessa unidade. A correção
agora insere a categoria diretamente nas listas plaintext `.emulationstation/es_systems.cfg`
e `es_systems--.cfg`, com backup e preservação estrutural de todos os demais sistemas.
Ainda é necessário repetir a abertura no console. Não houve execução confirmada do jogo.

O pacote corrigido contém **dois executáveis**, gnueabi e armhf, e seleciona o
runtime pelo loader disponível e caminho de bibliotecas herdado. Isso evita assumir
que a imagem SquashFS disponível no SD seja necessariamente o sistema inicializado
pela unidade. O build armhf foi ligado à SDL2 da raiz; 486 imports fortes foram
resolvidos na inspeção das bibliotecas hard-float presentes no cartão. O build
gnueabi manteve 527 imports resolvidos. Ambos usam baseline ARMv7/VFPv3.

A inspeção do SD mudou a hipótese inicial sobre ABI. O cartão inclui um menu
ARM hard-float na raiz e um conjunto diferente em `simulator/3168`. As imagens
SquashFS dessa distribuição inicializam o segundo conjunto. A imagem com
`Model Number:M17` informa `Board Model:W12D`, Buildroot 2021.11 e versão de
sistema 2025-04-01. Isso descreve arquivos do cartão, não uma medição da placa
que está dentro do aparelho nem comprovação de qual imagem ele inicializa.

O menu em `simulator/3168/emulationstation` e sua SDL2 usam ARM EABI soft-float
(ELF flags 0x05000200) com loader `/lib/ld-linux.so.3`. A libc da imagem tem a
mesma ABI. Por isso, **o toolchain m17-armhf.cmake não é adequado a este conjunto**.
O build adicional foi feito com Zig 0.14.1, target `arm-linux-gnueabi.2.29`,
baseline Cortex-A9/ARMv7/VFPv3, headers SDL2 2.30.11 e a biblioteca SDL2 já presente
nesse cartão. O C++ runtime é ligado estaticamente; suas licenças estão em
`third_party/`. As bibliotecas do firmware não foram redistribuídas.

A baseline ARMv7/VFPv3 evita exigir instruções de divisão do Cortex-A7 ou VFPv4;
a escolha é conservadora para os conjuntos ARMv7, não identificação do SoC real.
A SDL2 dinâmica do cartão fornece o backend de vídeo próprio. Não foi imposto
um driver genérico de desktop.

## Reprodução

Obtenha Zig 0.14.1 do fornecedor oficial ou pacote `ziglang==0.14.1`, extraia a
fonte oficial SDL2 2.30.11 e aponte para a SDL2 do cartão correspondente:

```sh
export ZIG=/caminho/zig
export SDL_SOURCE=/caminho/SDL2-2.30.11
export M17_SDL_LIBRARY=/caminho/SD/simulator/3168/lib_3168/libSDL2-2.0.so.0
sh scripts/build_card_arm.sh build-card-arm
```

O `SDL_config_minimal.h` é usado apenas para declarações públicas no build do
aplicativo; ele não substitui nem recompila o SDL do aparelho. A opção de linker
que permite dependências indiretas é acompanhada de verificação posterior:
527 imports fortes foram resolvidos, com versões, entre o executável e as
bibliotecas transitivas do conjunto do cartão e da imagem. Nenhuma dependência
forte ausente ou divergência de ABI foi encontrada nessa verificação estática.

## Integração específica

O executável do ES inspecionado contém `loadAdditionnalConfig`, que busca
`es_systems_*.cfg` em `/storage/.emulationstation`; a imagem possui `/storage`
e `/sdcard` apontando para o cartão. A primeira tentativa usou uma configuração adicional e falhou em aparecer no
console. A correção usa `scripts/repair_stock_es.py` para acrescentar um único
sistema às duas listas plaintext da raiz, com backup. A lista codificada em
simulator/3168, os scripts de boot e o firmware permanecem intocados.

O perfil `integration/m17-w12d/controls.sh` adiciona mapas SDL para os GUIDs
`i2c-keys` e `rk29-keypad` encontrados nos arquivos de entrada do ES. Mapeia os
botões direcionais para SDL GameController, inclusive movimento contínuo de Boca.
O mapa foi derivado dos arquivos; não foi confirmado apertando botões no M17.
O tema adicional inclui o tema M17 já no cartão e define os textos da categoria;
não redistribui os assets do tema de terceiros.

O pacote para esse cartão é **candidato a teste no dispositivo**, não um binário
universal de M17. Apenas a execução no console pode confirmar a imagem ativa,
backend de vídeo, controles físicos e retorno ao menu. Se não iniciar, recoloque
o SD no leitor e consulte `fake-mobile-games/run.log`. Para desfazer a correção, remova apenas o sistema de nome `fake-mobile-games`
das duas listas editadas, ou restaure seus backups se não houver mudanças posteriores.
A pasta do app e o subdiretório de tema também podem ser removidos.
