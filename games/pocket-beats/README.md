# Pocket Beats

**Status:** especificação aprovada para desenvolvimento; NÃO implementado/validado no M17.
**Prioridade:** próximo projeto após Block Out, por decisão do usuário em 20/10/2026.

A próxima etapa começa pela especificação de groovebox de 16 passos e quatro pistas; a implementação ainda não foi iniciada.

## Conceito e objetivo
Groovebox com sequenciador de 16 passos e quatro pistas: bateria, baixo, guitarra e synth.

## Mecânica e regras
BPM ajustável; 4 trilhas sincronizadas; padrões salváveis; volumes por pista; quantização; mute/solo; eventual WAV opcional.

## Controles propostos para M17
D-pad navega grade; A alterna passo; B volta; ombros escolhem instrumento/padrão; Start play/stop.

## Direção visual
Painel musical compacto e legível.

## Escopo MVP
- Menu inicial com Jogar, Pausar/Continuar, Reiniciar e Sair.
- Tutorial jogável de uma tela e três níveis/cenários com progressão.
- Interface e feedback legíveis na tela 480x272; suporte a navegação inteiramente pelos botões.
- Audio simples, opção de silenciar, placar/progresso local quando aplicável.
- Operação offline, sem telemetria externa.

## Observações
Projeto especial; mixagem de poucos canais e samples originais.

## Aceite
Jogo compilado para o ambiente Linux real do M17, regras reproduzíveis/testadas, controles físicos validados, sem mexer nos títulos existentes. Ver [CODEX.md](CODEX.md) para execução.
