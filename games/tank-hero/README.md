# Tank Arena

Status: especificação, ainda não implementado no M17.

## Objetivo e dinâmica
Tanques lutam em arenas com obstáculos e ricochetes leves.

## Regras completas do MVP
HP, recarga, parede impede projétil, inimigos IA, vitória após eliminar todos; 3 arenas. Jogador inicia em menu, vê tutorial, enfrenta pelo menos três níveis, recebe feedback de erro/acerto, pode pausar, reiniciar e sair. Não permitir estados impossíveis. Usar progresso local offline.

## Controles
direcionais mover; A disparar; B ajustar mira; Start pausa; Select retorna ao menu após confirmação. Validar botões reais do M17 e oferecer teclado no PC.

## Visual
Tanques pixel art 2D. Tela 480x272, sugerido canvas lógico 240x136 e escala 2x sem suavização. Arte original com identidade própria, inspirada na era 16 bits. Interface legível na tela pequena.

## Critérios de aceite
Loop jogável, três níveis válidos, controles físicos, build Linux reproduzível, testes de regras, FPS medido no M17. Não copiar assets nem código de outros jogos comerciais.
