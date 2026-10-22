# Validação — Color Pop 0.1.0, 22/10/2026

- Build nativo Apple Clang/C++11 com SDL2, `-Wall -Wextra -pedantic`, AddressSanitizer e UndefinedBehaviorSanitizer: sem avisos ou diagnósticos nos testes executados.
- Regressão completa da coleção: **34/34 grupos aprovados**, 52,05 s. O teste de regras estendido passou novamente após acrescentar mistura e vitória na última jogada.
- Quatro grupos próprios aprovados: regras/save/20 soluções, inicialização/desenho SDL sem tela, controles por eventos SDL, vitória/desbloqueio no fluxo real da aplicação.
- Regras: grupos adjacentes, ação inválida sem consumir jogada ou gerador, gravidade com origem das peças, reposição, pulso válido/inválido, derrota ao acabar jogadas, vitória na última jogada, mistura de tabuleiro sem jogadas, desfazer restaurando tabuleiro e estado aleatório.
- Campanha: as 20 sequências registradas são reproduzidas pelas regras e pela classe Game até cumprir ambas as metas, dentro do orçamento. Os testes conferem todos os desbloqueios e a gravação/retomada da fase 20. Não foi provada a solução mínima.
- Entrada: joystick SDL virtual com dois manches; teclado para A/B/Y, seleção, remoção, desfazer, pausa, retorno, saída e save. Outro teste resolve a primeira fase por botões, avança e carrega o save da fase seguinte.
- Visual: captura real SDL de 480×272; cores com símbolos distintos, grupo destacado, cursor, metas, combo, jogadas e carga do pulso conferidos.
- Build ARM: Zig, ARM gnueabi soft-float Cortex-A9, interpreter `/lib/ld-linux.so.3`, link com a SDL2 do runtime ativo recuperado da unidade W12D RK3168.
- Inspeção estática: 530 imports fortes e dependências transitivas resolvidos sem falhas; relatório em `arm-compatibility.json`. Bibliotecas do firmware não distribuídas.

**Hardware pendente:** FPS, latência, som real, legibilidade e mapeamento físico de botões não foram medidos no M17. Nenhum arquivo de SD/boot foi alterado.

## Roteiro no aparelho

Abrir Ports → Color-Pop-Test, confirmar instruções e testar os dois manches e D-pad. Estourar grupos com A, desfazer com B, formar grupos grandes até carregar o pulso e confirmar a cruz com Y/A. Conferir metas, queda, animação e som. Pausar com Start, voltar com B, sair com Select e reabrir para verificar a partida. Cumprir as duas metas e testar avanço de fase; esgotar jogadas em outra tentativa e testar reinício/desfazer. Confirmar retorno ao Ports e preservação dos outros jogos.
