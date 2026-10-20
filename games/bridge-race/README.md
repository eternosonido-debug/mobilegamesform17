# Bridge Builder Race

Status: especificação, ainda não implementado no M17.

## Objetivo e dinâmica
Coletar blocos da própria cor e erguer uma ponte até o objetivo.

## Regras completas do MVP
Coleta e pilha, consumir blocos automaticamente na ponte, competidor IA, chegada; 3 pistas. Jogador inicia em menu, vê tutorial, enfrenta pelo menos três níveis, recebe feedback de erro/acerto, pode pausar, reiniciar e sair. Não permitir estados impossíveis. Usar progresso local offline.

## Controles
direcionais mover; A ação contextual; Start pausa; Select retorna ao menu após confirmação. Validar botões reais do M17 e oferecer teclado no PC.

## Visual
Câmera superior simplificada. Tela 480x272, sugerido canvas lógico 240x136 e escala 2x sem suavização. Arte original com identidade própria, inspirada na era 16 bits. Interface legível na tela pequena.

## Critérios de aceite
Loop jogável, três níveis válidos, controles físicos, build Linux reproduzível, testes de regras, FPS medido no M17. Não copiar assets nem código de outros jogos comerciais.
