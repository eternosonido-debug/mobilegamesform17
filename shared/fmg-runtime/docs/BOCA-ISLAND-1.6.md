# Boca 1.6 — Ilha da Boca

## Ilha e câmera

A cidade ocupa uma ilha com costa irregular dentro do mundo de 3.200 × 2.000. Todos os limites são mar, com água rasa, espuma e praia entre o oceano e a área urbana. Ruas e calçadas terminam em terra. O jogador e os rivais não atravessam a costa; os rivais conseguem seguir pela lateral ao fugir junto à praia.

A câmera usa exatamente a posição da boca: ela fica no centro da área jogável (240,138 nas coordenadas lógicas), inclusive perto da costa. O oceano ocupa o espaço fora da ilha. O afastamento conforme a boca cresce permanece. O minimapa indica terra, mar e as quatro cores das bocas.

## Veículos

Os carros têm seis variantes visuais: compacto vermelho, sedã azul, táxi amarelo, SUV verde, ambulância branca com cruz e sinalizadores, e sedã roxo. Caminhões variam entre carga azul, carga ocre, bombeiros vermelho com escada e sinalizador, e carga verde. As variantes têm faces, cabine, vidros, rodas e sombras em 2.5D. O modelo deriva do identificador do objeto e permanece estável ao carregar o save e ao reaparecer. Ambulâncias usam a escala dos carros; bombeiros usam a dos caminhões.

## Partidas e instalação

Partidas da cidade 1.4/1.5 continuam abrindo no formato FMG 2. Na primeira retomada, posições que ficariam no mar são trazidas para a ilha. Tamanho da boca, pontuação, rivais, proteção e progresso são mantidos. Não é necessário reiniciar a rodada. Partidas novas e objetos que reaparecem já são posicionados dentro da costa.

Todos os recursos anteriores continuam: progressão de frutas até edifícios, objetivo de três edifícios, dez minutos por rodada, objetos reaparecendo, rivais se atacando e cinco segundos de proteção com a boca piscando.

A instalação substitui somente `fmg-test/bin/boca-test`, com backup prévio. O Juices Sort, atalhos Ports, controles e arquivos de save existentes são preservados durante a cópia. Abrir Ports → Boca-Test. Direcional move; A impulso; X ajuda; B volta; START menu; SELECT salva e sai.

## Validação

Build local com AddressSanitizer e UndefinedBehaviorSanitizer; quatro suítes de testes aprovadas: regras e saves anteriores, controles/retomada do Juices Sort, controles/save do Boca e cidade/progressão/rivais. Oito ilhas concluídas com movimento contínuo e rivais ativos. Testes adicionais de geração de objetos em terra, migração da cidade antiga, limite físico da costa e fuga pela borda. Prévias renderizadas da costa e dos novos veículos inspecionadas.

Binário ARM gnueabi compilado com a SDL2 copiada do cartão M17 W12D e verificado estaticamente. O desempenho da ilha e a câmera no aparelho ainda precisam de teste pelo usuário. As capturas locais não são testes em hardware.
