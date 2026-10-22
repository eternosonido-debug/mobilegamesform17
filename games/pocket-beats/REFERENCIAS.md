# Pocket Beats — referências verificadas

Pesquisa de 20/10/2026. Nenhum código externo foi incorporado ao Pocket Beats e nenhum cartão foi alterado nesta pesquisa.

| Projeto | O que pode ajudar | Situação de reaproveitamento |
|---|---|---|
| [Picoloop](https://github.com/yoyz/picoloop) | Sequenciador de quatro pistas, seleção de instrumentos, edição por controles e organização de padrões | Há motores e dependências de origens diferentes. Conferir licença do componente exato antes de copiar. O build para RG35XX SP usa AArch64 e não serve diretamente ao M17 ARM32. |
| [LittleGPTracker original](https://github.com/Mdashdotdashn/LittleGPTracker) | Tracker concebido para consoles portáteis; organização de samples, padrões e reprodução | Licença raiz BSD-3-Clause conferida. Bom candidato a reaproveitamento de componentes com avisos e créditos preservados; dependências precisam de conferência separada. A interface tracker não é automaticamente mais fácil para iniciantes. |
| [LittleGPTracker atualizado](https://github.com/djdiskmachine/LittleGPTracker) | Adaptação SDL2 e suporte documentado a outros portáteis | O fork atual declara GPLv3; não confundir com a licença BSD da versão original. |
| [TinySoundFont](https://github.com/schellingb/TinySoundFont) | Motor C/C++ para bancos SoundFont: possibilidade de timbres de baixo, guitarra e bateria baseados em samples | Biblioteca MIT, licença conferida. Unidade de implementação compilada no computador e para ARM gnueabi Cortex-A9 com a toolchain existente. Isso verifica compilação, não qualidade do banco, desempenho, execução ou som no M17. Licença do banco de sons é independente. |

## Direção sugerida

Para o som, avaliar TinySoundFont com um banco pequeno de licença de redistribuição clara e limite de vozes, comparando custo e som com nosso banco pré-calculado. Para a experiência de uso, estudar Picoloop e o LittleGPTracker original e adaptar seleção, navegação e nomes de notas ao Pocket Beats. Não portar uma estação de trabalho desktop completa nesta etapa.

Na revisão inicial do Pocket Beats, os números 1–8 dentro das células melódicas são índices de notas de uma escala pentatônica; não são volume, duração ou pontuação. A revisão 0.6 já usa nomes, atribuição direta de sete notas e A para ativar/desativar.

## Revisões inspecionadas

- Picoloop: `5c54d47364db61a9bb38c064bbcf5d1fed34879c`.
- LittleGPTracker original BSD: `4aca8cd765e1ad586da62decd019e66cb64b45b8`.
- LittleGPTracker fork GPL: `0b09128a6df92564406e7e6b5fdf3ff430df845b`.
- TinySoundFont: `853a0a171759f1ddba0de1442133a75912bbeffa`.

Validação realizada: leitura de documentação, licenças e partes de build/áudio; compilação isolada do TinySoundFont para computador e ARM32. Nenhum desses programas foi executado no M17 por nós nesta pesquisa.
