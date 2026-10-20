# Inventário recuperado — 20/10/2026

Fontes recuperados do Fake Mobile Games local, commit 816c4e0, versão 1.14. O código importado preserva as regras existentes; a reorganização só separa os módulos específicos, cria as entradas de build por jogo e mantém o motor legado compartilhado.

| Jogo | Pasta | Estado |
|---|---|---|
| Juices Sort | games/juices-sort | Ampliado; instalação em Ports registrada |
| New Blocks | games/new-blocks | Ampliado; instalação em Ports registrada |
| Color Mix | games/color-mix | Base inicial jogável; sem refinamento próprio |
| Sudoku | games/sudoku | Base inicial jogável; sem teste individual no aparelho registrado |
| Traffic Jam | games/traffic-jam | Base inicial jogável; sem refinamento próprio |
| Mahjong | games/mahjong | Visual e animações ampliados; instalação registrada |
| Boca | games/boca | Cidade, rivais, progressão e assets; instalação registrada |
| Color Loop | games/color-loop | 24 fases, coletores e animações; instalação registrada |

As cinco entradas efetivamente instaladas neste chat foram Juices Sort, Boca, New Blocks, Color Loop e Mahjong. Os três jogos básicos podem ser acessados pelo menu do pacote. A menção anterior do README ao Sudoku como instalado não foi confirmada pelos registros deste chat.

A revisão de controles 1.14 foi compilada e validada localmente, mas ainda aguardava instalação no cartão e confirmação dos dois manches físicos. Isso é diferente do estado das versões anteriores instaladas. Não há medição de FPS no hardware para esta importação.

## O que foi preservado

Os 17 diretórios de jogos planejados e seus documentos permanecem intactos. Block Out continua distinto de New Blocks. Nenhum save, dump de firmware, biblioteca proprietária, backup pessoal ou arquivo do SD é publicado.

A estrutura existente compartilhava regras e o loop SDL. Os arquivos específicos de cada jogo foram colocados em suas pastas; o restante está em shared/fmg-runtime para evitar cópias divergentes e preservar compatibilidade dos saves. As regras básicas ainda são métodos do motor legado. Desacoplar completamente as regras é trabalho futuro, não um resultado alegado desta importação.

O canvas existente é 480 × 272. A proposta de 240 × 136 do planejamento não foi imposta aos jogos já feitos. A arte aprovada dos jogos existentes também foi mantida.

## Validar esta organização

```sh
cmake -S . -B build -DCMAKE_PREFIX_PATH=/caminho/do/SDL2
cmake --build build
ctest --test-dir build --output-on-failure
```

Os testes incluem regras, saves, animações, controles e os dois manches virtuais em todos os oito jogos. O teste virtual não substitui validação dos eventos do firmware na unidade física.
