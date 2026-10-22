# Pesquisa anterior à implementação — 22/10/2026

Pedido: culinária acolhedora, estética de fazendinha fofa, identidade original. FarmVille é referência de clima/cores, sem reutilização de nomes, personagens, interface ou assets.

## GitHub inspecionado

- [MaronkaJozsef/CookingGame](https://github.com/MaronkaJozsef/CookingGame), revisão `a79ca5d60365b02be9ae3a0d795a25ad15458fc0`. Lidos README, `LICENSE.txt` e `project.godot` via GitHub. O projeto declara recriação de um tutorial Kitchen Chaos; a licença encontrada é MIT, com avisos Lyuma, lox9973, Hector Martin e Godot, sem atribuição clara de todos os assets/tutorial. Stack Godot 4.2, Forward Plus, cenas/GDScript e plugin de pós-processamento. Importar exigiria trocar o runtime ARM32/SDL2 já recuperado e esclarecer direitos de assets. Não copiar código ou arte; não executar esse projeto no M17.
- [Enginaaron/CAFECAFE](https://github.com/Enginaaron/CAFECAFE): consultada página/README. Usa Godot; a descrição diz MIT, mas não foi conferida uma licença completa cobrindo todos os assets. Não adotado nem copiado. Tema de sobrevivência/monstros e dependência de outro motor também não atendem ao escopo infantil/leve.

## Decisão antes de escrever o jogo

Reutilizar componentes próprios MIT (`LICENSE-FMG`) de Screw Puzzle: entrada configurável para dois manches, fonte bitmap, primitivas SDL, efeitos sonoros, launcher Ports e cross-build ARM comprovado. Novas regras puras, três receitas, personagem, cenário e animações serão originais e desenhados por código SDL, sem dependências de imagens externas. Renderizar em 480×272 nativos para contornos mais finos, com cenário estático em textura para reduzir redesenho. Sem reivindicar desempenho ou validação física antes do teste real.
