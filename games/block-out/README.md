# Block Out

**Status:** protótipo nativo implementado, compilado localmente e para ARM; **ainda não validado no M17 físico**. Versão inicial 0.1.0.

Puzzle de deslizar peças de montar até portas da mesma cor. O visual aprovado usa pixel art 16-bit, pinos, relevo e a mesma paleta de quatro cores. Não há números nas peças.

## Campanha de nove fases

| Fase | Grade | Peças | Obstáculos | Proposta |
|---|---|---|---|---|
| 1 | 6 × 6 | 2 | 2 | Tutorial de seleção e saída |
| 2 | 6 × 6 | 3 | 4 | Liberar o caminho de outro bloco |
| 3 | 6 × 6 | 4 | 4 | Combinar portas em vários lados |
| 4 | 6 × 6 | 5 | 4 | Rotas com mais peças |
| 5 | 6 × 6 | 5 | 5 | Espaço mais limitado |
| 6 | 6 × 6 | 6 | 6 | Mais bloqueios entre peças |
| 7 | 8 × 8 | 7 | 12 | Grade maior e peças quadradas |
| 8 | 8 × 8 | 7 | 14 | Mais corredores e obstáculos |
| 9 | 8 × 8 | 8 | 16 | Maior ocupação e ordem de retirada |

As fases são determinísticas e todas têm uma solução reproduzida nos testes. As três primeiras foram compostas manualmente; as demais são construídas de trás para frente, com caminhos reversíveis e solução registrada no código. Isso garante possibilidade de resolução, mas a curva percebida de dificuldade ainda depende de teste com jogadores. O número de movimentos da solução conhecida não é apresentado como mínimo ótimo.

A paleta não ganha novas cores. Nas fases cheias, mais de uma peça pode compartilhar a mesma cor, e qualquer porta compatível em largura e altura pode recebê-la. As peças mantêm sua orientação, sem rotação. Obstáculos e outras peças bloqueiam a movimentação. Uma saída inválida não muda o tabuleiro nem o contador.

## Animação aprovada

A peça desliza na direção da porta. A parte que ainda está dentro do quadrado permanece inteira e opaca; somente o que cruza a borda se desfaz em fragmentos. O efeito foi verificado comparando pixels nas quatro direções, nas duas grades e com portas que repetem a mesma cor.

## Controles

- Direcional ou qualquer um dos dois manches: move o cursor; com peça selecionada, desliza a peça.
- A: pegar / soltar.
- B: cancelar a seleção ou voltar da pausa.
- X: reservado para ataque, sem ação neste puzzle.
- Y ou Start: pausa e extras, com continuar, desfazer, reiniciar e som.
- Select: salvar e sair para Ports.

O menu inicial oferece jogar/continuar, tutorial, som e saída. Ao vencer, A avança e B repete. Ao concluir a campanha, A retorna ao menu. Não há cronômetro de derrota: o jogador pode tentar outras rotas e desfazer a última jogada.

## Save e áudio

Partida, fase, progresso, seleção, opção de som e uma jogada de desfazer são gravados em `block-out.sav`. O jogo salva após movimentos válidos, a cada cinco segundos e ao sair. Ao reabrir, a posição lógica é restaurada; uma saída que já foi confirmada não precisa repetir a animação. Os sons são sintetizados pelo próprio código e podem ser desligados. Se o backend de áudio não estiver disponível, o jogo continua sem som.

## Build, instalação e testes

Veja [BUILD.md](BUILD.md), [código de regras](src/game_logic.hpp) e [testes](tests). O pacote de Ports acrescenta apenas `Block-Out-Test.sh` e `block-out-test`, usando os controles existentes e um save próprio. Não substitui os jogos anteriores nem altera o menu original.

Os testes locais validam as nove soluções, ações inválidas, colisões, desfazer, save/resume, pausa, A/B/Y/Select, ambos os manches virtuais e a borda da animação. Compilação ARM e compatibilidade estática não equivalem a validação no hardware. FPS, som, legibilidade e eventos físicos ainda precisam ser conferidos na unidade.
