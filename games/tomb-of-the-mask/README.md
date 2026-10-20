# Mask Maze

Status: especificação, ainda não implementado no M17.

## Objetivo e dinâmica
Arcade de labirintos onde personagem desliza até bater na parede.

## Regras completas do MVP
Paredes, armadilhas, colecionáveis, saída, checkpoints; 3 labirintos. Jogador inicia em menu, vê tutorial, enfrenta pelo menos três níveis, recebe feedback de erro/acerto, pode pausar, reiniciar e sair. Não permitir estados impossíveis. Usar progresso local offline.

## Controles
direcionais disparam o deslize; A habilidade; Start pausa; Select retorna ao menu após confirmação. Validar botões reais do M17 e oferecer teclado no PC.

## Visual
Pixel art neon contrastante. Tela 480x272, sugerido canvas lógico 240x136 e escala 2x sem suavização. Arte original com identidade própria, inspirada na era 16 bits. Interface legível na tela pequena.

## Critérios de aceite
Loop jogável, três níveis válidos, controles físicos, build Linux reproduzível, testes de regras, FPS medido no M17. Não copiar assets nem código de outros jogos comerciais.
