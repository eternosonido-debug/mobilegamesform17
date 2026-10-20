# Color Loop — primeira versão

**Controles atuais:** a versão 1.14 passa dicas, ajuda e desfazer para **Y → Extras**, reservando X para ataques. Consulte [CONTROLS.md](CONTROLS.md) para o padrão vigente; os controles descritos abaixo registram a versão de lançamento.

Jogo original do Fake Mobile Games inspirado no núcleo de seleção e coleta do Sort Loop, da Velha Games. Nome de trabalho: Color Loop. O objetivo é remover todos os blocos enviando coletores da cor certa para uma esteira em circuito fechado.

## Regras implementadas

- Quatro filas. Apenas o coletor da frente de cada fila pode entrar; a próxima peça é revelada ao enviá-lo.
- Quatro espaços simultâneos na esteira, com intervalo de entrada para evitar sobreposição.
- Cada coletor leva seis itens de sua própria cor. O número no personagem indica quantos faltam.
- Ao passar por cada lado do tabuleiro, só pode recolher o primeiro bloco exposto naquela direção. Cores diferentes bloqueiam o acesso aos blocos atrás delas.
- O coletor completo deixa o circuito e libera a vaga.
- Após uma volta, o incompleto vai para um dos quatro espaços de retorno. Reenvie-o para terminar a coleta.
- Se outro coletor precisa voltar com todos os espaços de retorno ocupados, a fase termina. É possível desfazer o último envio ou tentar novamente.
- Vence ao remover todos os blocos. São 24 fases determinísticas, com duas a quatro cores e tabuleiros de 36, 48 ou 60 itens.
- Cada coleta vale 10 pontos; completar um coletor dá mais 50.

A descrição oficial confirma escolha do Hopper, colocação em slot, entrada na esteira e coleta automática por cor. Ela não especifica capacidade, exposição, duração da volta ou regras exatas de retorno. Os números e regras acima são decisões desta adaptação; não foram apresentados como reprodução verificada do original. Referência consultada: https://play.google.com/store/apps/details?id=com.velhagames.sortloop

Resgates, fusão de personagens, moedas e expansão da base descritos no original não fazem parte desta primeira versão. A arte e o código são próprios do projeto.

## Controles

Direcional escolhe a fila ou o retorno. Cima/baixo alterna entre as duas áreas; esquerda/direita alterna a coluna. A envia o coletor selecionado; X abre ajuda; B ou Y fecha a ajuda. Y durante a partida desfaz o último envio, restaurando também a posição dos coletores e a coleta daquela jogada.

Ao vencer, A avança e B repete. Ao perder, A ou B tenta novamente; Y desfaz o envio. Start abre o menu geral, que pausa a fase; Select salva e sai para Ports. Não há cronômetro de derrota.

Partida, fase e progresso são salvos automaticamente a cada cinco segundos e ao sair, em `fmg-test/saves/game-7.sav`. Inclui posições de todos os coletores em movimento, filas, retorno e quantidade restante de cada cor. O maior nível liberado fica salvo junto da partida. Esta versão avança sequencialmente e ainda não oferece seleção de fases antigas.

## Instalação no M17 já configurado

Copie `fmg-test/bin/color-loop-test` para o mesmo caminho no cartão e `.config/emuelec/ports/Color-Loop-Test.sh` para a pasta Ports existente. Não altere os arquivos de sistemas ou de inicialização do EmulationStation. O launcher reaproveita `fmg-test/controls.sh` e o runtime SDL 2.28.5 do M17 já identificado. Abra **Ports → Color-Loop-Test**.

Não substitui os executáveis de Boca, Juices Sort ou New Blocks. O pacote não precisa de imagens externas.

## Build e validação

Build local usa o CMake habitual do projeto, com SDL2. O mesmo binário aceita `--game color-loop`. Para o M17, use o alvo ARM gnueabi soft-float Cortex-A9 e as bibliotecas do aparelho documentadas no projeto.

Foram executados nove grupos de testes locais. Os específicos cobrem todas as 24 fases concluídas em simulação, bloqueio por cores na frente, capacidade da esteira, intervalo de entrada, lotação do retorno, desfazer, save/resume exato de coletores em movimento e eventos SDL reais de controles/ajuda/saída. As regras e os testes dos outros jogos também passaram.

Esta versão ainda não foi executada no hardware M17. Compatibilidade estática ARM e resultados no computador não equivalem a teste de fluidez no aparelho.

## Atualização 1.12 — movimento e ritmo

A esteira tem setas que correm continuamente no sentido dos coletores, mesmo quando vazia. O percurso usa curvas arredondadas e velocidade visual uniforme. O ritmo aumentou de quatro para seis segmentos por segundo; uma volta leva entre quatro e 5,3 segundos, conforme o tamanho da fase.

Os personagens têm orelhas, pés, olhos com brilho, piscadas, sorriso e reação ao recolher. A entrada faz um salto até o circuito; o retorno incompleto é animado, e a conclusão solta partículas e um indicador de pontos. A animação usa o próprio desenho SDL, sem novas texturas externas.

Um toque em A durante o bloqueio da entrada ou com a esteira cheia agenda um único envio: ele entra assim que houver espaço, mantendo o coletor na fila até lá. B cancela o envio agendado. O comando agendado também é salvo e pode ser desfeito com Y. A escolha não muda se você mover o cursor depois de agendar.

Dez grupos de testes locais passaram, incluindo comparação de quadros da esteira vazia, continuidade da velocidade nas curvas, envio agendado/cancelamento e as 24 fases concluídas novamente em simulação. Medição local do desenho com SDL software: cerca de 0,38 ms por quadro em 100 quadros; este resultado é do computador e não representa o desempenho no M17.

Saves anteriores continuam compatíveis. Boca, New Blocks e Juices Sort não precisam ser substituídos para receber esta atualização.

## Seleção reforçada 1.13

O coletor selecionado fica elevado, com seta acima e borda dourada forte. Os outros coletores selecionáveis ficam com menor opacidade. Vale tanto para as filas quanto para os espaços de retorno.
