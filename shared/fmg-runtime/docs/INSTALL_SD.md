# Instalação via SD e categoria EmulationStation

A instalação é de um aplicativo Linux nativo: nenhum emulador ou ROM é usado.
A categoria "Fake Mobile Games" tem sete entradas que lançam o mesmo executável
com jogos diferentes. O aplicativo também oferece seu próprio menu.

1. Identifique firmware/revisão, caminho de montagem do cartão no console,
   SDL2 e ABI conforme [M17.md](M17.md). Faça cópia dos arquivos de configuração
   que pretende editar. Não é necessário gravar imagem ou mudar boot para copiar o app.
2. Compile o binário ARM com o SDK correspondente, conforme [BUILD_M17.md](BUILD_M17.md).
3. Desligue o console e monte o SD no computador. Execute o instalador com o
   caminho do cartão no PC e seu caminho **no console**, que são diferentes:

```sh
python3 scripts/install_sd.py --sd /media/usuario/SD \
  --binary build-m17/fake-mobile-games --runtime-root /sdcard
```

Para um cartão cuja partição aparece como `/userdata` no console, use
`--runtime-root /userdata`. Não assuma isso a partir do nome da partição no PC.

## Conteúdo gerado

```text
fake-mobile-games/
  bin/fake-mobile-games
  launch.sh
  controls.sh.example
  es_systems.fragment.xml
  games/*.fmg                 # 7 manifestos com identificadores permitidos
  games/gamelist.xml
  lib/                       # dependências ARM opcionais do mesmo SDK
  saves/                     # progresso persistente
```

O instalador rejeita executáveis macOS/x86 e verifica o cabeçalho ELF32 ARM;
isso não é prova de compatibilidade de libc/ABI. Não inclui falsas bibliotecas
nem um executável ARM de demonstração.

## Criar categoria própria

Encontre a configuração realmente utilizada pelo seu EmulationStation. Em
upstream, pode ser `~/.emulationstation/es_systems.cfg` ou `/etc/emulationstation/es_systems.cfg`;
no firmware do M17, descubra pelos arquivos e scripts existentes. No Batocera,
confirme a configuração personalizada em `/userdata/system/configs/emulationstation/`
na versão utilizada. Não substitua uma lista completa por um único sistema.

Se o arquivo que o ES usa estiver no SD, passe seu caminho **relativo ao cartão**:

```sh
python3 scripts/install_sd.py --sd /media/usuario/SD \
  --binary build-m17/fake-mobile-games --runtime-root /sdcard \
  --es-config .emulationstation/es_systems.cfg
```

Esse comando mescla apenas o sistema `fake-mobile-games`, preserva os outros e
cria um backup com sufixo `.fmg-backup-...`. Se o arquivo não existir, cria uma
lista nova: use isso somente quando tiver confirmado que é uma configuração
personalizada válida para seu firmware. A opção sem `--es-config` nunca altera
configuração ES; apenas produz `es_systems.fragment.xml` para revisão e mesclagem.

O comando da entrada gerada tem esta forma:

```xml
<command>/bin/sh "/sdcard/fake-mobile-games/launch.sh" "%ROM%"</command>
```

Adapte o caminho real. Em cartão FAT, `chmod` pode não ser persistente; invocar o
launcher com `/bin/sh` contorna o bit do script, mas o binário ainda precisa estar
em montagem executável. Em montagem `noexec`, escolha local executável suportado
pelo firmware; copiar arquivos não contorna essa restrição.

Reinicie o ES ou atualize a lista. Um tema sem suporte para o novo sistema pode
usar apresentação genérica. Os nomes vêm de gamelist.xml. Se a categoria não
aparecer, confirme arquivo lido, extensão `.fmg` e caminhos antes de editar outros
scripts. No stock com lista fixa, a categoria pode exigir modificar o frontend;
esse trabalho específico de firmware não foi validado nem é feito automaticamente.
Um frontend com Ports configurável pode servir como alternativa de lançamento:
chame `/bin/sh /caminho/fake-mobile-games/launch.sh`, sem argumento, para o menu.

## Primeira execução e desinstalação

Antes de usar o ES, execute o binário em um shell do próprio console, se disponível:

```sh
FMG_SAVE_DIR=/sdcard/fake-mobile-games/saves \
FMG_FULLSCREEN=1 /sdcard/fake-mobile-games/bin/fake-mobile-games --input-test
```

Mapeie os controles em `controls.sh`; veja [CONTROLS.md](CONTROLS.md).
O launcher grava erros em `run.log`. Não seleciona à força backend SDL, para
respeitar a imagem instalada. Se necessário configure um backend compatível em
`controls.sh`. Sem vídeo funcional, não há promessa de execução nativa.

Para remover, saia do jogo, guarde `saves/`, remova a pasta do aplicativo e
remova somente o `<system>` de nome `fake-mobile-games` da configuração ES,
ou restaure o backup se nenhum outro sistema tiver sido alterado depois dele.
