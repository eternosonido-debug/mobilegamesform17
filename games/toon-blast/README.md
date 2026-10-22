# Color Pop 0.1.0

Jogo original de eliminar grupos, nativo C++11/SDL2, com 20 fases offline. A pasta `toon-blast` é o nome histórico da referência da curadoria; o executável, o save e a entrada em Ports se chamam **Color Pop**. Não utiliza personagens, arte, sons ou níveis do aplicativo de referência.

## Como jogar

Selecione um grupo de duas ou mais peças da mesma cor, unidas pelos lados, e aperte A. As peças estouram, as que estão acima caem e outras entram no topo. Diagonais não conectam grupos. Não há troca de peças nem eliminação por linhas.

Cada fase pede duas cores, mostradas à direita com o progresso e a quantidade necessária. Cumpra ambas antes de acabar as jogadas. Remover uma peça isolada não gasta jogada. Se cumprir a meta na última jogada, a vitória tem prioridade. Não há vidas, compras ou esperas: pode reiniciar ou desfazer.

Grupos de quatro ou mais aumentam o multiplicador, até 4x, e carregam o **pulso**. Ao atingir 12 de energia, Y prepara uma explosão em cruz; os destaques mostram a linha e a coluna atingidas. A confirma a explosão, que custa uma jogada e consome a carga. B cancela a preparação. A pontuação de cada ação é `quantidade² × multiplicador`. Grupos pequenos e pulsos voltam o combo para 1x.

Se a reposição produzir um tabuleiro sem grupos válidos, ele é misturado automaticamente sem gastar jogada extra. Após oito tentativas, um par é garantido. A sequência de reposição usa um gerador próprio determinístico, e seu estado é salvo: desfazer e retomar não mudam os resultados.

## Controles

| Entrada | Função |
|---|---|
| D-pad / qualquer um dos dois manches | Selecionar célula |
| A | Estourar grupo / confirmar pulso / confirmar menu |
| B | Desfazer; com pulso preparado, cancelar; voltar dos menus |
| Y | Preparar pulso quando carregado; na derrota, desfazer |
| Start | Pausar; B retorna à partida |
| Select | Salvar e sair para Ports |
| X | Sem função nesta versão |

Teclado: setas, Z/espaço=A, X=B, V=Y, Enter=Start e Escape=Select. A camada configurável respeita `FMG_KEY_*`, `FMG_BUTTON_*` e o mapeamento dos dois direcionais da coleção. O launcher carrega o `controls.sh` existente. Botões reais ainda precisam de teste físico.

O menu permite continuar, reiniciar, trocar entre fases liberadas, silenciar e sair. A primeira partida mostra as instruções. Ao vencer, A avança para a próxima. Após a fase 20, aparece a conclusão da campanha. Ao perder, A tenta novamente, Y desfaz se existir histórico da sessão e B volta ao menu.

## Campanha

- Fases 1–5: três cores e metas de 6 a 10 peças de cada cor indicada.
- Fases 6–12: quatro cores e metas de 11 a 17.
- Fases 13–20: cinco cores e metas de 18 a 25.
- Tabuleiro de 7×6; orçamento de 12 a 21 jogadas, conforme a fase.

Cada fase possui semente fixa e uma sequência vencedora reproduzida pelos testes. As quatro últimas sementes foram selecionadas após a busca encontrar soluções dentro do orçamento. `campaign-solutions.json` registra sementes e ações; `tests/solutions.hpp` contém os mesmos testemunhos. São provas de que existe solução, não de solução mínima nem de dificuldade estritamente crescente. A avaliação humana de dificuldade e diversão ainda está pendente. Metas e variedade aumentam ao longo da campanha.

## Save e implementação

`color-pop.save` guarda tabuleiro, gerador de reposição, fase, seleção, desbloqueios, metas coletadas, jogadas, pontuação, combo, energia e som. A escrita usa arquivo temporário e rename; uma falha aparece na tela. O histórico de até 64 ações e a preparação do pulso são locais à sessão. Saves inválidos são rejeitados, iniciando partida nova. Nenhum save de outro jogo é lido ou alterado.

Regras/gravidade/save em `src/game_logic.hpp`, desenho e animações em `src/renderer.hpp`, entrada/sons/fontes nos outros cabeçalhos, aplicação em `src/main.cpp`. Os desenhos e os sons são próprios, sob [LICENSE-FMG](../../LICENSE-FMG). A pesquisa anterior à implementação está em [REFERENCIAS.md](REFERENCIAS.md). A busca de soluções fica nos testes; não roda no M17 durante o jogo.

Veja [BUILD.md](BUILD.md) e [VALIDACAO.md](VALIDACAO.md). **Versão compilada para ARM e validada no computador; ainda não executada no M17 físico.** Esta entrega não instalou arquivos no cartão.
