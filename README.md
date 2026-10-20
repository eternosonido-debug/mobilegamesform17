# Mobile Games for M17

Coleção de jogos originais para Linux no portátil M17. Registro inicial de decisões em 20/10/2026.

## Fontes recuperados neste repositório

Oito jogos existentes já possuem pasta, fontes, entrada de build e documentação: **Juices Sort, New Blocks, Color Mix, Sudoku, Traffic Jam, Mahjong, Boca e Color Loop**. Consulte [inventário verificado](docs/INVENTARIO-RECUPERADO.md), [controles atuais](docs/CONTROLS.md) e [build M17 recuperado](docs/M17-RECUPERADO.md).

O registro inicial abaixo é histórico. Suas pendências sobre localizar fontes e identificar os cinco Ports foram esclarecidas no inventário. A revisão mais recente dos controles ainda depende de teste físico.

## Estado confirmado pelo usuário
- Cinco jogos já funcionam no M17. Não substituir ou alterar esses projetos até recuperar os respectivos fontes.
- Nomes citados entre os existentes: Juicy Sort, Boca (antes Fominha/Hole.io), Sudoku, Mahjong. O quinto e a correspondência completa devem ser conferidos no repositório/ambiente original, sem suposição.
- Protótipo de Block Out feito em HTML; **ainda não foi portado nem validado no M17**.
- Direção visual Block Out aprovada: puzzle de deslizar blocos até portas da mesma cor; pixel art 16-bit inspirada em consoles clássicos, sem copiar franquias.
- Prévia visual Juicy Sort aprovada: laranja fatiada gigante em pixel art, parcialmente cortada na borda; outros menus/capas serão feitos depois.

## Jogos novos selecionados na curadoria
1. Block Out
2. Screw Puzzle
3. Rotate Rings
4. Magic Tiles (ritmo, considerar quatro botões frontais e quatro superiores)
5. Pocket Beats (groovebox de 16 passos; bateria, baixo, guitarra, sintetizador)
6. Paper.io (mecânica original inspirada em conquista de território)
7. Slither.io (evitar duplicar Snake Clash)
8. Toon Blast (mecânica própria de grupos de blocos)
9. Helix Jump
10. Going Balls
11. Stack Ball
12. Tomb of the Mask
13. Archero
14. Bridge Race
15. Tank Hero
16. Labirinto: avaliar conceito final entre referências Labyrinth e Maze Machina, evitando duplicatas
17. Minichef (culinária, recuperar conceito anterior)

## Ordem inicial aprovada
Block Out → Screw Puzzle → Rotate Rings → Toon Blast → Stack Ball → demais.

## Pendências técnicas
- Localizar fontes e scripts de build dos cinco jogos existentes e identificar o compilador/ambiente Linux do M17.
- Portar o Block Out de protótipo HTML para o mesmo stack nativo adotado nos jogos existentes; testar controles e desempenho no console.
- Não afirmar que jogos novos já rodam no aparelho sem testes.
- Criar menu próprio e capa de cada jogo somente após consolidar seleção e implementação.

## Estrutura proposta
- `games/`: fontes dos títulos, quando recuperados/criados.
- `assets/`: artes e capas.
- `docs/`: decisões, portabilidade e instruções de instalação.


## Direção de arte e arquitetura compartilhada (20/10/2026)

- **Estética**: pixel art com referência à era 16-bit. É uma linguagem visual, não uma limitação técnica obrigatória para todos os títulos.
- **Tela-alvo do M17**: 480 × 272. Proposta de canvas lógico de **240 × 136**, escala inteira 2x por nearest-neighbor (sem suavização), a validar no aparelho.
- **Identidade comum**: menus, navegação, tipografia, feedback sonoro, controles e telas de pausa coerentes.
- **Identidade individual**: paleta, ilustração, mecânica e capa específicas por jogo. Evitar que Block Out se confunda com New Tetris.
- **Menu Juicy Sort**: referência aprovada com laranja fatiada pixelada gigantesca, aproximadamente um quarto visível, cortada pela borda.
- **Block Out**: blocos e portas correspondentes em cores, pixel art retrô aprovada. Não confundir com jogo de formar linhas.
- **Assets reutilizáveis**: atlas de sprites e componentes genéricos; texturas de interface, fontes, ícones de controle, sons de interface e transições. Não reutilizar personagens/arte protegida de aplicativos de referência.
- **Performance**: sprites em baixa resolução, áudio simples e carregamento leve. Medir desempenho real antes de prometer compatibilidade.
- **Controles**: padronizar mapa SDL de D-pad, A/B, Start/Select e ombros L1/L2/R1/R2 conforme detecção do M17; cada jogo pode especializar as funções. Evitar exigir toque.
- **Entrega por título**: mecânica funcional → teste no M17 → refinamento visual → menu próprio → capa no seletor. Não confundir protótipo HTML com binário Linux instalado.
