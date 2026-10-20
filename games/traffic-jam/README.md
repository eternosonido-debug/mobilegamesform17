# Traffic Jam

Implementação básica jogável do pacote original; ainda sem o refinamento visual dos demais.

## Estado recuperado

- Fonte importado do Fake Mobile Games local, versão 1.14, preservando as regras.
- Disponível pelo menu compartilhado; não há instalação individual nem teste no hardware registrado neste chat.
- Testes locais do conjunto: 13 grupos aprovados, incluindo ambos os manches virtuais nos oito jogos. Não equivale a teste no M17.
- Canvas atual: 480 × 272. A proposta nova de 240 × 136 não foi aplicada aos jogos existentes.
- Save próprio: `game-4.sav`. Não são publicados saves pessoais.

## Build

```sh
cmake -S games/traffic-jam -B build/traffic-jam -DCMAKE_PREFIX_PATH=/caminho/do/SDL2
cmake --build build/traffic-jam
./build/traffic-jam/traffic-jam
```

Também pode compilar tudo pela raiz. SDL2 e C++11 são necessários. Para M17, use a toolchain e o runtime ARM gnueabi descritos em [M17 recuperado](../../docs/M17-RECUPERADO.md). Não usar uma biblioteca SDL hard-float com esse executável.

## Organização dos fontes

`src/main.cpp` abre diretamente este jogo. Os módulos específicos recuperados ficam em `src/`. As regras antigas e o loop SDL ainda compartilham o motor preservado em `shared/fmg-runtime`: esta importação não reescreve nem duplica o motor. Nos jogos básicos, as regras continuam nos métodos correspondentes de `shared/fmg-runtime/src/game.hpp`. A separação completa das regras é uma evolução futura, não uma alteração feita nesta entrega.

[Controles atuais](../../docs/CONTROLS.md) · [Inventário e limites](../../docs/INVENTARIO-RECUPERADO.md)
