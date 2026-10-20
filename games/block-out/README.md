# Block Out

**Status:** especificação aprovada para desenvolvimento; NÃO implementado/validado no M17.
**Prioridade:** 1 — primeiro jogo.

## Conceito e objetivo
Deslizar blocos de cores distintas por uma grade com obstáculos até a porta da mesma cor.

## Mecânica e regras
Fases determinísticas e solucionáveis; parede bloqueia; saída aceita apenas bloco correspondente; vitória quando todos saem; contador de movimentos; desfazer 1 jogada.

## Controles propostos para M17
Selecionar bloco por setas; A para pegar/soltar; setas para deslocar na grade; B cancelar; Start pausar.

## Direção visual
Pixel art 16-bit de tijolos e portais; contrastar com New Tetris.

## Escopo MVP
- Menu inicial com Jogar, Pausar/Continuar, Reiniciar e Sair.
- Tutorial jogável de uma tela e três níveis/cenários com progressão.
- Interface e feedback legíveis na tela 480x272; suporte a navegação inteiramente pelos botões.
- Audio simples, opção de silenciar, placar/progresso local quando aplicável.
- Operação offline, sem telemetria externa.

## Observações
Primeiro jogo. Aproveitar protótipo HTML como referência, não como versão Linux.

## Aceite
Jogo compilado para o ambiente Linux real do M17, regras reproduzíveis/testadas, controles físicos validados, sem mexer nos títulos existentes. Ver [CODEX.md](CODEX.md) para execução.
