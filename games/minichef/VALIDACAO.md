# Validação — Minichef 0.1.0, 22/10/2026

## Executado no computador

- Compilação C++11/SDL2 com Apple Clang, `-Wall -Wextra -pedantic`, AddressSanitizer e UndefinedBehaviorSanitizer, sem avisos de compilação.
- Regras: três receitas concluídas da seleção à decoração; ingredientes errados, corte fora do verde, direção incorreta e bloqueio de receitas ainda não liberadas; condição de cocção/mexidas e pontuação/desbloqueio. Uma simulação de entradas perfeitas leva 16/19/22 segundos ativos; isso não é duração humana ou tempo mínimo de fase. Não há espera artificial até três minutos.
- Save/retomada a cada passagem de simulação, incluindo marcador de corte, temperatura e decoração. Rejeição de arquivo truncado/índice fora do limite/decoração inválida; falha não substitui estado já carregado. Passos fixos produzem o mesmo estado agrupados em quadros diferentes.
- Smoke com renderer SDL software e áudio dummy.
- Controles de integração: dois manches virtuais (eixos 0 e 2), seleção com A, ajuda com Y/B, pausa/retomada com Start/B, Select com confirmação, saída e save. Os eixos verticais e D-pad usam o componente genérico já existente; não constituem mapeamento físico comprovado.
- Integração de vitória: retoma save válido na decoração, usa três ações A, mostra vitória, inicia receita seguinte e verifica o desbloqueio salvo.
- Regressão geral: 45/45 grupos passaram em 65,57 s antes da inclusão do grupo de vitória Minichef; depois, os quatro grupos específicos do Minichef foram executados com sanitizadores.
- Prévias PNG capturadas do renderer SDL do jogo, em 480×272, no computador. Não são fotografias nem capturas do M17.

## ARM

`build-m17.sh` gerou ELF ARM32 soft-float Cortex-A9 usando Zig e o SDL 2.28.5 recuperado da unidade M17 W12D/RK3168. Inspeção estática de 536 importações fortes e dependências/versionamentos do runtime sem falhas; relatório `arm-compatibility.json`. Isso não executa o jogo nem confirma suporte ao driver de vídeo/áudio.

## Pendente no aparelho

M17 físico: FPS/carga, legibilidade em LCD 4,3”, conforto dos dois manches, mapeamento real A/B/Y/Start/Select, som, retomada após fechar e retorno ao Ports. Nenhuma instalação ou escrita no SD/boot nesta entrega. Campanha inicial curta de três receitas; sem gerenciamento de restaurante, plantações persistentes, multiplayer ou música de fundo.
