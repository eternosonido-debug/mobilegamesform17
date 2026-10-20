# Tower Break

Status: especificação, ainda não implementado no M17.

## Objetivo e dinâmica
Destruir segmentos de plataformas na descida de uma torre.

## Regras completas do MVP
Queda e rebote; evitar segmentos perigosos; combos; vitória na base; 3 torres. Jogador inicia em menu, vê tutorial, enfrenta pelo menos três níveis, recebe feedback de erro/acerto, pode pausar, reiniciar e sair. Não permitir estados impossíveis. Usar progresso local offline.

## Controles
A mantido para cair; soltar para parar; Start pausa; Select retorna ao menu após confirmação. Validar botões reais do M17 e oferecer teclado no PC.

## Visual
Camadas coloridas e estilhaços. Tela 480x272, sugerido canvas lógico 240x136 e escala 2x sem suavização. Arte original com identidade própria, inspirada na era 16 bits. Interface legível na tela pequena.

## Critérios de aceite
Loop jogável, três níveis válidos, controles físicos, build Linux reproduzível, testes de regras, FPS medido no M17. Não copiar assets nem código de outros jogos comerciais.
