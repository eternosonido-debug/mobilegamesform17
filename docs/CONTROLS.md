# Controles — padrão 1.14

Todos os oito jogos compartilham a mesma leitura de movimento: direcional digital, manche esquerdo e manche direito. A movimentação é equivalente em cada um. Nos jogos de tabuleiro, o cursor repete a cada 140 ms; no Boca, o movimento é contínuo e a velocidade diagonal é normalizada. Start abre o menu, e Select salva e sai.

## Botões durante a partida

| Jogo | A: ação principal | B: ação secundária | Y: extra |
|---|---|---|---|
| Juices Sort | Selecionar / despejar | Cancelar seleção | Dica, desfazer, fases, ajuda e reinício |
| New Blocks | Colocar peça | Trocar peça | Desfazer, ajuda e reinício |
| Color Mix | Alterar linha | Alterar coluna | Desfazer, ajuda e reinício |
| Sudoku | Escrever número | Próximo número | Apagar célula, desfazer e reinício |
| Traffic Jam | Selecionar / soltar carro | Cancelar seleção | Desfazer, ajuda e reinício |
| Mahjong | Selecionar / retirar par | Cancelar seleção | Dica, desfazer e reinício |
| Boca | Impulso | Nova cidade após o fim | Ajuda, impulso e reinício |
| Color Loop | Enviar coletor | Cancelar envio agendado | Desfazer, ajuda e reinício |

X fica reservado para ataques durante a partida. Os jogos atuais não possuem um golpe manual; no Boca, a absorção e o confronto continuam automáticos por contato. X não abre mais dicas ou ajuda durante a partida. No menu geral, X ainda inicia uma partida nova.

Y abre Extras. Use qualquer direcional para escolher, A para confirmar e B para voltar. O jogo fica pausado enquanto esse menu está aberto. O Juices Sort usa seu painel de campanha, agora com Desfazer. Telas de vitória mantêm A para prosseguir / começar outra mesa e B para repetir quando disponível.

## Leitura dos dois manches

A leitura SDL aceita eixos esquerdo e direito de GameController, eixos brutos e todos os hats. Um mapa de GameController que só descreve botões não esconde mais os eixos brutos. A deadzone padrão é 16000. Valores de eixos brutos podem ser configurados em `controls.sh`:

```sh
export FMG_AXIS_X=0 FMG_AXIS_Y=1
export FMG_AXIS_RX=2 FMG_AXIS_RY=3
export FMG_DEADZONE=16000
```

São valores de fallback, não índices medidos dos dois manches na unidade M17. Para um GameController com eixos mapeados, o mapa SDL tem prioridade, a menos que uma dessas variáveis seja explicitamente definida. Cada eixo é conferido contra a quantidade de eixos que o dispositivo realmente expõe.

Se um firmware expuser o segundo manche como teclas próprias, configure `FMG_KEY_RIGHT_LEFT`, `FMG_KEY_RIGHT_RIGHT`, `FMG_KEY_RIGHT_UP`, `FMG_KEY_RIGHT_DOWN`. Teclas independentes continuam aceitando os dois direcionais mesmo quando pressionados ao mesmo tempo. As variáveis antigas `FMG_KEY_LEFT/RIGHT/UP/DOWN` e de A/B/X/Y permanecem compatíveis.

O mapeamento dos botões físicos já usado no cartão é preservado. Não foi necessário alterar `controls.sh` para habilitar a leitura adicional de eixos.

## Limite da verificação

O M17 possui dois manches físicos. O que cada um emite depende do firmware. Se dois controles físicos emitirem exatamente o mesmo evento (por exemplo, manche direito e um botão de ação), a aplicação não consegue identificar qual foi pressionado só por esse evento. Nesse caso, é necessário verificar o driver ou um mapeamento que forneça sinais separados. A presença de dois manches não confirma que a unidade fornece quatro eixos independentes.

Foram executados testes SDL com controles virtuais de quatro eixos em todos os oito jogos, nas duas direções, usando mapas completos e mapas apenas de botões. O teste confirma cursor/movimento, ausência de ação acidental e gravação. Os treze grupos de testes locais passaram. Não foi alegado teste dos manches no aparelho.

## Diagnóstico na unidade

Cada inicialização registra nome, quantidade de eixos, hats e botões no log do jogo. `--input-test` também imprime `KEY`, `BUTTON`, `AXIS` e `HAT` com identificação do dispositivo. Pressione cada manche separadamente e depois A/B/X/Y. Select/Esc sai.

O launcher opcional `integration/ports-test/Controles-Teste.sh` usa uma pasta de saves de diagnóstico separada. Só instale essa entrada em Ports se precisar registrar os eventos do aparelho. Os resultados ficam em `fmg-test/controles-teste.log`. Não modifique o mapa global ou os arquivos de inicialização do EmulationStation para fazer esse teste.
