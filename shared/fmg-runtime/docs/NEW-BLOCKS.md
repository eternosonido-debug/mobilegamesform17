# New Blocks

**Controles atuais:** a versão 1.14 passa dicas, ajuda e desfazer para **Y → Extras**, reservando X para ataques. Consulte [CONTROLS.md](CONTROLS.md) para o padrão vigente; os controles descritos abaixo registram a versão de lançamento.

Quebra-cabeça de encaixe sem limite de tempo. Coloque as três peças disponíveis em um tabuleiro 8 × 8. Linhas e colunas completas desaparecem simultaneamente. Após usar o trio, recebe-se outro. A rodada termina quando nenhuma peça restante cabe em qualquer posição do tabuleiro.

## Controles

- Direcional: move a origem da previsão de encaixe.
- A: coloca a peça. Verde indica posição válida; vermelho indica impedimento.
- B: alterna entre as peças disponíveis.
- Y: desfaz a última jogada, inclusive após o fim da rodada.
- X: abre ajuda durante a partida; após o fim, inicia outra rodada.
- A após o fim: nova rodada.
- Start: menu geral.
- Select: salva e sai para o Ports.

Ao pontuar, um indicador com o ganho sobe e desaparece em 0,75 segundo; linhas completadas recebem um brilho breve.

Há 16 formas, incluindo peças pequenas, barras, cantos, quadrados e retângulos. A orientação de cada forma é fixa. Completar mais de uma linha na mesma jogada aumenta o bônus; limpar em jogadas consecutivas dá combo. Cada célula colocada vale um ponto, cada célula removida vale dois, e limpar L linhas dá mais 10 × L² pontos, além de 10 por nível de combo após o primeiro.

Partida e recorde são gravados em `fmg-test/saves/game-1.sav`, separados dos outros jogos. O recorde permanece ao desfazer ou iniciar outra rodada. Salva automaticamente a cada cinco segundos e ao sair pelo menu/Select. Não remova o cartão durante uma gravação.

## Instalação no cartão já configurado

Copie `fmg-test/bin/new-blocks-test` para o diretório correspondente da partição de jogos. Copie `New-Blocks-Test.sh` para `.config/emuelec/ports/`. O launcher usa a mesma biblioteca SDL e o mesmo `fmg-test/controls.sh` que já funcionaram no aparelho. Não é preciso alterar a configuração de sistemas do EmulationStation. Abra **Ports → New-Blocks-Test**.

Este pacote pressupõe a instalação M17 já existente neste projeto; o binário ARM gnueabi soft-float foi compilado para Cortex-A9 e verificado estaticamente contra as bibliotecas extraídas do aparelho. Esta versão do New Blocks ainda não foi executada no hardware.

## Validação

Testes de todas as formas, limpeza simultânea de linha/coluna, bloqueio de posições inválidas, detecção de ausência de encaixe, desfazer, preservação do recorde e 50 partidas com sementes distintas. Teste SDL com eventos reais de teclado cobrindo colocação, desfazer, troca de peça, ajuda e gravação. As regras anteriores dos outros jogos também foram verificadas.
