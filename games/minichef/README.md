# Minichef 0.1.0 — Cozinha da Horta

Culinária acolhedora, original, offline, C++11/SDL2 para o M17 Linux. Coelhinho cozinheiro, jardim na janela, alimentos desenhados por código, madeira clara, aventais, cortinas e paleta pastel. Atmosfera de fazendinha solicitada pelo usuário, com personagens/cenário próprios e sem assets de FarmVille.

## Três receitas jogáveis

| Receita | Ingredientes | Cortes | Mistura | Tempo no ponto |
|---|---|---:|---:|---:|
| Sopa da Horta | cenoura, tomate, batata | 6 | 8 direções | 10 s |
| Panqueca do Sol | banana, farinha, ovo | 8 | 10 direções | 13 s |
| Tortinha Berry | farinha, ovo, morango | 10 | 12 direções | 16 s |

Esses tempos são apenas o requisito da etapa de cocção; não são uma estimativa de duração humana da receita. A campanha inicial é curta e não tem limite de tempo. Cada receita concluída desbloqueia a seguinte; é possível refazer receitas para melhorar de uma a três estrelas.

1. **Colher:** escolha os três ingredientes indicados na lista da cozinha. Itens errados não entram na cesta; feedback gentil, sem perder o preparo.
2. **Cortar:** aperte A quando o marcador estiver no verde. A janela fica um pouco menor a cada receita. Cada corte requer um novo toque e intervalo mínimo de 1/3 s; segurar A não completa a etapa.
3. **Misturar:** siga as setas com qualquer direcional. A sequência gira em torno da tigela; a próxima direção só conta com novo movimento.
4. **Fogão:** esquerda/direita ajustam o fogo de 0 a 4. Mantenha o termômetro no verde para cozinhar e use A para mexer quatro vezes (intervalo mínimo de um segundo). Fora do ponto o preparo não avança; mexer antes da hora não conta. O fogão é deliberadamente tolerante, sem alimentos perdidos.
5. **Decorar:** escolha folhinhas, flores ou creme e coloque três enfeites. O prato final recebe estrelas e libera a próxima receita.

Não há game over, punição por esperar ou limite de tentativas. Até dois erros dão três estrelas; até oito, duas; acima disso, uma. O melhor resultado é preservado. Erros de corte/ingrediente/mistura têm sinal visual e sonoro; ações durante o pequeno intervalo de corte/mexida são ignoradas.

## Controles

- D-pad ou **qualquer um dos dois manches**: selecionar ingredientes, seguir setas, ajustar fogo ou decoração. Todos funcionam pelo mesmo caminho de regras.
- **A:** pegar, cortar, mexer, decorar ou confirmar.
- **B / Start:** pausar; B retoma a partida no menu de pausa.
- **Y:** ajuda contextual, com o jogo parado; A/B voltam.
- **Select:** confirmação para salvar e retornar ao menu. No menu, salva e sai para Ports.
- X e ombros não são exigidos. Teclado: setas, Z/espaço=A, X=B, V=Y, Enter=Start, Escape=Select.

No menu, selecione a receita e use esquerda/direita para mudar entre as desbloqueadas; A começa essa receita. Continuar mantém o preparo salvo. Recomeçar limpa somente a tentativa atual. Perder o foco pausa automaticamente. Entrada configurável por `controls.sh`/variáveis `FMG_KEY_*`, `FMG_BUTTON_*` e eixos genéricos da coleção.

## Visual, animações e som

Desenho original em **480×272 nativos**, com traços menores que o canvas 240×136 dos jogos anteriores. A cozinha e os ícones ficam em texturas estáticas para reduzir o trabalho por quadro. Personagens se movimentam suavemente; faca, colher, vapor, fervura, corações e confetes acompanham o preparo. Três pratos têm desenhos próprios. Efeitos sonoros sintetizados leves; sem música de fundo nesta versão. Nenhum arquivo de imagem/fonte/áudio externo é necessário.

## Salvamento

`minichef.save`, formato CHEF1, escrita em temporário + rename: receita, etapa, cesta, corte, mistura, temperatura, fogo, cocção, mexidas, decoração, erros, melhores estrelas, desbloqueios e opção de som. Salva a cada mudança de etapa, pausa, saída e a cada 15 segundos ativos. Uma interrupção abrupta pode perder até 15 segundos; saída normal preserva o ponto atual. Saves truncados/fora dos limites são rejeitados sem alterar o objeto já carregado. Diretório configurável por `FMG_SAVE_DIR`, padrão SDL_GetPrefPath.

Código/arte procedural próprios sob [LICENSE-FMG](../../LICENSE-FMG). Componentes genéricos próprios reutilizados de Screw Puzzle; [pesquisa e decisão](REFERENCIAS.md). [Build/instalação](BUILD.md) e [evidências/limites](VALIDACAO.md).

**Em desenvolvimento: compilado para ARM, mas ainda não executado no M17 físico.** Nenhuma escrita no SD nesta entrega. Legibilidade dos textos menores, qualidade visual no LCD, mapeamento físico, áudio e FPS precisam de teste no aparelho.
