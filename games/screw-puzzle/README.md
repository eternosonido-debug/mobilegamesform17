# Screw Puzzle 0.1.0

Primeira versão jogável nativa C++11/SDL2 para a coleção M17. Três fases originais, placas metálicas sobrepostas, retirada animada dos parafusos, queda das placas, efeitos sonoros, pausa, desfazer e progresso salvo. Implementação offline, sem mouse ou toque.

## Como jogar

Solte todas as placas. Um parafuso retirado fica na mão: encaixe-o em um furo livre antes de retirar outro. Uma placa cai quando perde seu último parafuso. Material sólido das placas superiores esconde parafusos e furos inferiores; remover a placa revela o que está por baixo. Placas que já caíram não voltam a prender parafusos.

- D-pad ou qualquer um dos dois manches: selecionar posição.
- **A:** retirar ou encaixar o parafuso; confirmar menus.
- **B:** cancelar a retirada enquanto segura um parafuso; caso contrário, pausar/voltar.
- **Y:** desfazer uma ação, incluindo a queda que ela provocou.
- **Start:** pausar/continuar pelo menu.
- **Select:** salvar e sair para Ports.
- **X:** sem função nesta versão.

No computador: setas, Z/espaço=A, X=B, V=Y, Enter=Start, Escape=Select. Os mesmos ajustes `FMG_KEY_*`, `FMG_BUTTON_*` e direcionais da coleção são respeitados; o launcher reaproveita `controls.sh` da unidade. Os controles físicos ainda precisam de confirmação no M17.

O menu permite continuar, reiniciar, trocar entre fases desbloqueadas, desligar o som e sair. A primeira entrada mostra instruções. Não há tempo limite nem vidas: pode desfazer ou reiniciar sem punição. A vitória desbloqueia a fase seguinte. Ao concluir a terceira, aparece a conclusão da campanha.

## Fases verificadas

| Fase | Placas | Parafusos | Estacionamentos livres iniciais | Solução BFS encontrada |
|---|---:|---:|---:|---:|
| 1 | 2 | 4 | 2 | 7 ações |
| 2 | 3 | 7 | 2 | 13 ações |
| 3 | 4 | 10 | 3 | 19 ações |

Uma ação é uma retirada ou um encaixe. O contador de giros conta somente retiradas. A busca automatizada verifica cada ação da solução e a conclusão; a dificuldade percebida ainda precisa de avaliação humana. A gravidade é uma animação simples, sem simulação física de balanço em um único apoio.

O save `screw-puzzle.save` guarda fase, seleção, estado do tabuleiro, parafuso na mão, progresso e som. A gravação usa arquivo temporário e substituição por rename. O histórico de desfazer existe apenas durante a sessão; reiniciar o programa mantém a partida, mas limpa esse histórico. Se um save inválido for encontrado, o jogo inicia uma partida nova. Não lê nem altera saves de outros títulos.

## Arquivos e validação

Regras e soluções em `src/game_logic.hpp`, desenho/animação em `src/renderer.hpp`, entrada em `src/input.hpp`, som em `src/audio.hpp` e aplicação em `src/main.cpp`. Os componentes genéricos próprios foram reaproveitados do Block Out sem alterar o jogo anterior. Arte e sons são gerados por código próprio, sob [LICENSE-FMG](../../LICENSE-FMG).

Veja [BUILD.md](BUILD.md), [VALIDACAO.md](VALIDACAO.md) e a [pesquisa prévia de reutilização](REFERENCIAS.md). Há executável ARM e pacote Ports; **a execução desta versão no M17 físico permanece pendente**. Não foi instalado no cartão nesta entrega.
