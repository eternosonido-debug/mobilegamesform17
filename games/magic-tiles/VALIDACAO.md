# Revisão 0.2.0 — dez músicas

Os quatro grupos locais passaram com sanitizadores: 30 charts concluídos perfeitamente, eventos de áudio sincronizados para as dez faixas, callback independente, pausa, controles e migração TILES1 → TILES2 preservando recordes. As três composições anteriores mantêm seus mapas; sete novas têm motivos, tonalidades, progressões e BPM próprios. Cada faixa contém 180 segundos de música, além de contagem inicial e cauda. Validação musical percebida e execução no M17 permanecem pendentes.

# Validação — Magic Tiles 0.1.0, 22/10/2026

- Compilação Apple Clang/C++11, SDL2, `-Wall -Wextra -pedantic`, AddressSanitizer e UndefinedBehaviorSanitizer: sem avisos ou diagnósticos nos testes.
- Quatro grupos próprios: regras/charts/save, desenho SDL sem tela, controles/menu, áudio/relógio.
- Nove charts concluídos nas regras com 100% de acertos e 1.000.000 pontos. Testes verificam duração superior a 180.000 ms, vitória somente no fim, limites de julgamento, notas perdidas, derrota, treino e persistência/rejeição de save inválido.
- Ajustes de −150/+150 ms testados contra o fim bruto do áudio: não permitem vitória antes da duração e não impedem o encerramento da faixa.
- Áudio: todo evento de nota do chart difícil corresponde ao evento musical da mesma amostra/instante; três buffers sintetizados com energia não nula. Callback avança com a thread da interface esperando; pausa mantém relógio e contador de amostras; retomada avança e o relógio final alcança exatamente a duração declarada.
- Controles: dois manches SDL virtuais, quatro botões de pista, escolha de música/dificuldade, ajuste de atraso, treino/som, pausa, retorno ao menu, saída e retomada das configurações. Isso não confirma botões reais nem percepção de latência.
- Regressão completa: 42/42 grupos da coleção aprovados em 60,62 s. Teste de áudio estendido passou novamente após acrescentar o encerramento exato do relógio.
- Visual: captura real do renderer SDL de um estado de jogo simulado aos 15 s; pistas, notas, linha, A/B/X/Y, vida, combo e pontuação conferidos. Prévia WAV de 12 s gerada pelo mesmo sintetizador, sem assets externos.
- ARM: Zig, arm-linux-gnueabi.2.29 Cortex-A9 soft-float, interpreter `/lib/ld-linux.so.3`, SDL2 do runtime ativo recuperado da unidade W12D RK3168. Relatório estático de bibliotecas/imports em `arm-compatibility.json`; não substitui execução.

**Pendente no hardware:** conforto por três minutos, legibilidade, qualidade musical percebida, latência e estabilidade do áudio sob carga, FPS, mapeamento físico e retorno ao Ports. Nenhuma escrita no SD/boot. Modo por ombros/pistas extras não implementado nesta primeira versão.

## Roteiro no M17

Abrir Ports → Magic-Tiles-Test, escolher Fácil e Treino, iniciar e tocar A/B/X/Y na linha. Testar os dois manches em movimentos curtos, esquerda/baixo/cima/direita. Ajustar ATRASO se os acertos parecerem adiantados/atrasados. Pausar e retomar, verificar se o áudio continua sem cortes durante o jogo e se a pausa congela as notas. Completar uma música sem Treino para gravar recorde; sair com Select, reabrir e conferir o recorde/configurações. Testar normal/difícil e confirmar retorno ao Ports.
