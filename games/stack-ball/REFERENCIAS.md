# Pesquisa anterior à implementação — Tower Break, 22/10/2026

Consultados GitHub e base própria: buscas `stack ball game clone`, `stack ball language:C#` e SDL; arquivos de licença/build/README dos candidatos abaixo.

| Projeto/revisão inspecionada | Evidência e decisão |
|---|---|
| [Bytenol/MetalBall-Crush-3D](https://github.com/Bytenol/MetalBall-Crush-3D), `207ef07dff2a34762e232c04f81db52ef2e8329a` | C++20/Emscripten/WebGL2/GLES3; CMake e package.json incluem Vite/nodemon. Não foi identificada licença de reutilização na árvore consultada. Não copiar fonte, modelos, shaders ou imagens nem assumir compatibilidade com o runtime SDL2 do M17. |
| [Edward-Khaymanov/Stack-Ball](https://github.com/Edward-Khaymanov/Stack-Ball), `a1ff62507052dd5a188cb84f6642d96d98419a0e` | Unity 2021.3.6f1/C#, Addressables, ShaderGraph, UGUI e física; README/manifest conferidos. Não foi identificada licença de reutilização na árvore. Não copiar código, skins ou níveis. |
| [KonradLinkowski/Stack](https://github.com/KonradLinkowski/Stack), `484292536dfdc7488b0aa5efc39ac63b519b1bda` | LICENSE MIT, TypeScript/Three.js/Vite. É outra mecânica (empilhar blocos), sem base SDL2 ou queda por plataformas. Não adaptar esse stack de navegador para o aparelho. |

Nenhum candidato foi executado no M17. A pesquisa não demonstra inexistência de outras opções. A ausência de licença identificada impede tratar um repositório público como fonte livre para cópia.

## Reutilização escolhida antes do código

Reutilizar entrada configurável, direcionais, fonte/primitivas, efeitos sonoros, scaffolding de menu/save e build/Ports próprios da coleção (Screw Puzzle, LICENSE-FMG). Criar regras, torres, física fixa, cilindro/plataformas e partículas originais. Nenhum código, asset, shader ou nível externo será incorporado. O nome público será Tower Break; `stack-ball` permanece apenas como pasta histórica da especificação.
