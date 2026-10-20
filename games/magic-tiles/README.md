# Magic Tiles

**Status:** especificação aprovada para desenvolvimento; NÃO implementado/validado no M17.
**Prioridade:** fila posterior.

## Conceito e objetivo
Jogo rítmico de notas descendo por pistas sincronizadas à música.

## Mecânica e regras
Mapa de tempo em milissegundos; janelas de acerto; combo; precisão; seleção de dificuldade; calibração de latência.

## Controles propostos para M17
A/B/X/Y pistas básicas; ombros acrescentam pistas ou modificadores em modo avançado.

## Direção visual
Colunas destacadas com notas grandes.

## Escopo MVP
- Menu inicial com Jogar, Pausar/Continuar, Reiniciar e Sair.
- Tutorial jogável de uma tela e três níveis/cenários com progressão.
- Interface e feedback legíveis na tela 480x272; suporte a navegação inteiramente pelos botões.
- Audio simples, opção de silenciar, placar/progresso local quando aplicável.
- Operação offline, sem telemetria externa.

## Observações
Usar músicas próprias ou licenciadas; testar taxa de atualização e latência.

## Aceite
Jogo compilado para o ambiente Linux real do M17, regras reproduzíveis/testadas, controles físicos validados, sem mexer nos títulos existentes. Ver [CODEX.md](CODEX.md) para execução.
