# Color Pop

**Status:** especificação aprovada para desenvolvimento; NÃO implementado/validado no M17.
**Prioridade:** 4.

## Conceito e objetivo
Eliminar grupos adjacentes de dois ou mais blocos da mesma cor.

## Mecânica e regras
Flood-fill por grupo; gravidade vertical; novas peças; metas de fase por movimentos; combo e boosters originais.

## Controles propostos para M17
D-pad célula; A remover grupo.

## Direção visual
Blocos em pixel art arredondada e efeitos curtos.

## Escopo MVP
- Menu inicial com Jogar, Pausar/Continuar, Reiniciar e Sair.
- Tutorial jogável de uma tela e três níveis/cenários com progressão.
- Interface e feedback legíveis na tela 480x272; suporte a navegação inteiramente pelos botões.
- Audio simples, opção de silenciar, placar/progresso local quando aplicável.
- Operação offline, sem telemetria externa.

## Observações
Diferenciar de New Tetris e Block Out por grupos, não por linhas.

## Aceite
Jogo compilado para o ambiente Linux real do M17, regras reproduzíveis/testadas, controles físicos validados, sem mexer nos títulos existentes. Ver [CODEX.md](CODEX.md) para execução.
