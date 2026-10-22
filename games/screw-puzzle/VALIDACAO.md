# Validação — 0.1.0, 22/10/2026

- Compilação nativa Apple Clang, C++11, SDL2, `-Wall -Wextra -pedantic`, AddressSanitizer e UndefinedBehaviorSanitizer: concluída sem avisos ou diagnósticos nos testes.
- Três grupos próprios: regras/save/soluções; inicialização e desenho SDL sem tela; fluxo de controles SDL com joystick virtual de quatro eixos.
- Regras: ações válidas e inválidas, suporte até o último parafuso, queda, exposição de parafuso inferior, desfazer/cancelar, vitória das três fases, persistência, rejeição de save inválido sem alterar estado carregado.
- Controles: navegação pelos eixos dos dois manches virtuais, teclado, retirada/encaixe, desfazer, cancelar, pausar, voltar, sair e carregar o save produzido. Não substitui teste de botões reais.
- Revisão visual: capturas reais do renderer SDL em 480×272 das fases 1 e 3. UI, posições sobrepostas, seleção e legenda conferidas.
- Regressão da coleção: 26/26 grupos de teste aprovados na compilação completa (36,86 s). Os três grupos próprios também passaram com sanitizers após a revisão final.
- ARM: Zig, `arm-linux-gnueabi.2.29`, Cortex-A9, EABI soft-float; interpreter `/lib/ld-linux.so.3`. Link com a SDL2 exata recuperada do runtime ativo W12D RK3168. Bibliotecas proprietárias/firmware não incluídas no pacote.
- Inspeção estática: 530 imports fortes e dependências transitivas resolvidos contra o runtime recuperado; nenhuma falha. Relatório `arm-compatibility.json` acompanha este documento.

**Não executado no M17 nesta entrega.** FPS, latência, áudio real, mapeamento físico, legibilidade na tela do aparelho e retorno ao Ports precisam ser conferidos. Nenhum arquivo do cartão/boot foi alterado.

## Roteiro no aparelho

1. Abrir Ports → Screw-Puzzle-Test; confirmar instruções com A.
2. Selecionar parafuso, retirar com A, encaixar em furo livre e verificar o som.
3. Repetir com cada manche e D-pad; B cancela e Y desfaz.
4. Soltar uma placa, verificar queda e revelação das camadas inferiores.
5. Pausar com Start, retornar com B, sair com Select e reabrir para conferir save.
6. Completar as três fases e verificar desbloqueio/conclusão. Confirmar que os outros Ports continuam disponíveis.
