# Pesquisa antes da implementação — 22/10/2026

Pesquisados GitHub (busca de repositórios: rotate rings puzzle, rotating rings puzzle SDL, ring puzzle game) e a infraestrutura desta coleção antes de criar o código nativo.

| Projeto / revisão inspecionada | Licença e stack | Decisão |
|---|---|---|
| [Krishita17/cogwind](https://github.com/Krishita17/cogwind), `2a09473939827df9196274dae594b095f1f1e59f` | LICENSE MIT, Swift/SwiftUI, watchOS; README e árvore conferidos. | Alinhamento de glifos em anéis concêntricos, outra mecânica. Não traz um port SDL2/ARM32; não copiar código/assets. |
| [Meldiron/rotation-puzzle-game](https://github.com/Meldiron/rotation-puzzle-game), `fc5a5e1a4dadca926e378766bd3fd88e6c6ac7f5` | LICENSE MIT para código JS/browser; README exclui fotos e som dessa licença. | Puzzle de girar imagens, não desencaixar anéis. Não reutilizar fotos/som nem importar um navegador para o M17. |
| [VelbazhdSoftwareLLC/Fish-Rings-for-Android](https://github.com/VelbazhdSoftwareLLC/Fish-Rings-for-Android), `dc3807e7d405843453afc798c6cc02205b65e1fb` | LICENSE GPLv3; Java/Android, Gradle. README e build raiz conferidos. | Puzzle de permutar peças, outra mecânica; não incorporar código ou recursos neste módulo MIT. |
| [SimonHung/NineLinkedRings](https://github.com/SimonHung/NineLinkedRings), `f9cacccf6add7a91256b073a3e027382187a39fa` | JavaScript/KineticJS, sem licença de reutilização na árvore consultada. | Outra mecânica (nove argolas chinesas); não copiar sem permissão verificável. |

Não foi encontrado nestes candidatos um jogo pronto SDL2/ARM32 que reduza mais o trabalho do que a infraestrutura própria existente. A busca não comprova inexistência de outros projetos. Nenhum candidato foi executado no M17.

## Reutilização escolhida antes de implementar

Reutilizar entrada configurável para dois manches/D-pad, fonte/primitivas, efeitos sonoros, scaffolding SDL, menu/save e scripts de build/Ports próprios da coleção (Screw Puzzle, LICENSE-FMG). Criar geometria, regras, fases e desenhos próprios. Nenhum código, nível, som ou imagem dos candidatos externos será copiado ou traduzido. Dependência de runtime: somente SDL2 e bibliotecas já usadas pela coleção; sem física pesada.
