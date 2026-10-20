# Validação da entrega — 19/10/2026

## Executado neste ambiente

Host macOS arm64, Apple clang, C++11, CMake 4.4.4 e SDL2 2.30.11 compilado da
fonte oficial. Nenhuma biblioteca ou toolchain foi instalada no sistema global;
dependências de validação ficaram fora do projeto entregue.

- Compilação CMake do executável SDL e testes: passou, sem avisos no código do projeto.
- Testes de regras e persistência: **25.490 verificações passaram**.
- AddressSanitizer e UndefinedBehaviorSanitizer: testes de regras e executável SDL
  compilados com ambos; execuções concluídas sem diagnósticos.
- **8 execuções headless SDL**, uma do menu e uma de cada jogo, 3 frames por execução,
  renderer software, driver dummy: saída 0 e captura BMP não vazia em todas.
- Capturas convertidas para PNG e inspecionadas visualmente. Uma revisão tornou a
  grade vazia de New Blocks visível; as oito capturas foram regeneradas.
- **7 testes Python** do instalador e launcher passaram.
- `sh -n scripts/launch.sh`: passou.

### Cobertura das regras

- 100 seeds × 7 jogos: estado inicial válido e round-trip de save/load.
- Juices Sort: 25 distribuições geradas solucionadas por busca; despejo em tubo
  vazio, transferência de grupo e bloqueio por cor/capacidade.
- New Blocks: encaixe, bordas, colisão e limpeza de linha.
- Color Mix: ciclos de linha/coluna e detecção do objetivo.
- Sudoku: 12 variações com solução única contadas por busca; solução aceita e
  duplicata rejeitada.
- Traffic Jam: bloqueios/eixos e busca até vitória; 976 estados visitados no teste.
- Mahjong: 100 seeds com os 16 pares removidos em uma sequência legal cada.
- Boca: absorção, expiração de tempo e crescimento capaz de absorver os 60 objetos.
  O teste posiciona a boca nos objetos: verifica as regras, não habilidade humana.
- Desfazer, arquivo truncado sem modificar estado de destino, rejeição de estado
  inválido e 3.000 entradas aleatórias por jogo preservando invariantes.

### Cobertura de instalação

Os testes usam diretórios temporários e um cabeçalho ELF ARM sintético somente
como fixture; esse arquivo **não é distribuído nem considerado um binário do jogo**.
Validam backup e mesclagem idempotente do XML, preservação de outro sistema,
7 manifestos, rejeição de executável desktop e caminhos fora do cartão, XML
inválido antes de copiar, preservação de saves/controles, lançamento do menu,
passagem de `--game boca` e rejeição de manifesto fora da whitelist.

## Não executado

- Build Linux desktop e workflow remoto GitHub (o workflow foi entregue como arquivo).
- Cross-compile ARM: sem SDK/sysroot correspondente ao firmware do usuário.
- Execução no M17, framebuffer/KMS, ABI/libc do aparelho, mapeamento físico,
  EmulationStation de firmware real, medição de FPS/RAM/temperatura/bateria.
- Sessões completas jogadas por humano; testes usam regras e renderização automática.
- Corte de energia durante gravação, SD físico ou montagem FAT/noexec.

**Resultado:** código-fonte jogável, compilado e validado no host nos limites acima.
Portabilidade ao M17 foi preparada e documentada; compatibilidade no dispositivo
continua pendente. Nenhum executável ARM ou selo de teste em hardware é alegado.

## Reproduzir

```sh
cmake -S . -B build-check -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_CXX_FLAGS='-fsanitize=address,undefined -fno-omit-frame-pointer'
cmake --build build-check -j2
ctest --test-dir build-check --output-on-failure
python3 -m unittest discover -s tests -p 'test_*.py' -v
sh -n scripts/launch.sh
SDL_VIDEODRIVER=dummy SDL_RENDER_DRIVER=software \
  ./build-check/fake-mobile-games --game new-blocks --smoke --screenshot blocks.bmp
```

No macOS, informe `CMAKE_PREFIX_PATH` para seu SDL2 se necessário. O modo dummy
não valida exibição física. As imagens em `screenshots/` são do renderer real do
programa; não são mockups.

## Atualização após inspeção do SD

Foi compilado um binário ARM gnueabi com Zig 0.14.1 para a SDL2 do conjunto
`simulator/3168` do cartão. Verificação estática de ABI, loader, bibliotecas e
527 imports fortes passou. Isso substitui a limitação anterior "sem build ARM"
para esse perfil específico; execução ARM e teste no hardware continuam pendentes.
Os testes de regras e os 7 testes Python foram repetidos após o ajuste que cria
a janela já em tela cheia a 480 × 272. Um smoke test com fullscreen e driver
dummy passou no host. O relatório inicial acima descreve a entrega anterior.

## Correção após categoria ausente no console

O usuário informou que a categoria não apareceu na primeira tentativa. Não foi
criado `run.log`, indicando que o launcher não chegou a registrar uma abertura.
A causa exata do arquivo adicional não ser aplicado não foi comprovada. Foram
acrescentados dois executáveis (ARM gnueabi e armhf) com seleção de runtime e uma
mesclagem direta nas duas listas plaintext de sistemas existentes. Nenhum boot
script foi alterado. 10 testes Python passaram, incluindo três testes de
preservação de outras categorias, idempotência e rejeição de XML inválido.
A verificação estática armhf resolveu 486 imports fortes sem falhas; gnueabi
resolveu 527. Testes de regras no host passaram. Falta testar a correção no console.
