# Pesquisa prévia — Color Pop, 22/10/2026

Pesquisa realizada antes da implementação: GitHub por `samegame SDL2`, `samegame language:C++` e `toon blast clone`, além da base própria desta coleção.

| Projeto/revisão conferida | Licença, dependências e adequação |
|---|---|
| [chausner/sgbust](https://github.com/chausner/sgbust), `132453539cbaaa204bf0a3f719425e6c63ef36b7` | LICENSE MIT (Christoph Hausner, 2024). README e CMake: solucionador CLI C++20, TBB, CLI11, mdspan, mimalloc, parallel_hashmap e wyhash. Resolve SameGame, sem o refill/metas desta especificação. Não importar seu stack para o runtime ARM/C++11. |
| [batuhanh/ToonBlast_Casual_Clone](https://github.com/batuhanh/ToonBlast_Casual_Clone), `313738abe2acfe964512e5a493648d7c1f946c6e` | LICENSE MIT, Unity/C#, módulos de render, áudio, animação e física no manifest. README descreve DFS de grupos e editor de fases. Não é executável SDL2/M17. Não importar arte, sons, níveis ou identidade da referência comercial. |
| [MehmetCanCaliskan/ColorSplash](https://github.com/MehmetCanCaliskan/ColorSplash), `75cc722611c4213676dd13e8d86b2d87e69b695d` | LICENSE MIT, Unity/C# com pacotes 2D, UGUI, Timeline e Visual Scripting. README apresenta um clone; não é base nativa ARM32/SDL2. Assets não adotados. |

Não foi executado nenhum candidato no M17. Não foi encontrado nestes candidatos um port que economize mais trabalho que a infraestrutura própria já testada. Isso não comprova inexistência de outra opção.

## Decisão anterior ao código

Reutilizar os componentes próprios de entrada para ambos os manches, fonte/primitivas, sons, scaffolding SDL, menu/save e scripts ARM/Ports de Screw Puzzle, sob LICENSE-FMG. Criar flood-fill, gravidade/refill determinístico, metas, fases e desenhos próprios. Nenhum código, nível, sprite ou som externo será copiado ou traduzido. O jogo público se chama Color Pop; `toon-blast` permanece apenas como pasta histórica da especificação.
