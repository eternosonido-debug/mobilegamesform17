# Validação — Tower Break 0.2.0, 22/10/2026

- Compilação Apple Clang/C++11 com SDL2, avisos `-Wall -Wextra -pedantic`, AddressSanitizer e UndefinedBehaviorSanitizer; testes sem diagnósticos.
- Quatro grupos próprios aprovados: regras/torres/save; inicialização/desenho SDL sem tela; controles; retomada perto da base com vitória e desbloqueio no fluxo da aplicação.
- Duração: torres de 640/680/720 andares. Limites inferiores calculados sem perigos de 186,59/198,26/209,92 s, todos maiores que 180 s. Simulações completas: 32.895/35.685/46.530 passos de 1/120 s. Teste verifica tempo mínimo e retomada do marco de 80 andares.
- Regras: três torres concluídas por uma política de ataques seguros, presença de setores seguros/perigosos em cada plataforma, morte ao atacar perigo, rebote no perigo sem atacar, combos/pontuação, estado terminal sem continuar a simulação, save/retomada e rejeição de dados inválidos.
- Tempo: estados idênticos ao agrupar os mesmos passos de física em frames simulados de 30 e 60 Hz. Isso verifica o modelo; não mede FPS no aparelho.
- Controles: A mantido por teclado e botão de joystick virtual, botão bruto alternativo FMG_BUTTON_A, tecla alternativa FMG_KEY_A, limpeza ao perder foco, eixos dos dois manches, pausa/retorno e confirmação de saída com save.
- Fluxo SDL: carregar perto da base da torre 1, quebrar o último andar, mostrar vitória, avançar para a torre 2 e retomar o save com desbloqueio.
- Regressão completa da coleção: 38/38 grupos aprovados, 55,72 s. Os quatro próprios passam novamente após o ajuste do limite de velocidade e distância entre quebras.
- Revisão visual: captura real SDL de 480×272, setores vermelhos listrados, bola, painel de perigo e área do cenário recortada para não cobrir a legenda.
- ARM: Zig, ARM gnueabi soft-float Cortex-A9, interpreter `/lib/ld-linux.so.3`, SDL2 do runtime ativo recuperado da unidade W12D RK3168. Inspeção estática sem falhas, detalhada em `arm-compatibility.json`.

**Sem execução no M17 nesta entrega.** FPS, latência, som, botões e conforto reais precisam de teste no aparelho. Não houve escrita no SD, no boot ou nos outros jogos. A política automatizada de conclusão não é um teste de dificuldade humana.

## Roteiro no M17

Abrir Ports → Tower-Break-Test. Confirmar o tutorial, soltar A e pressionar novamente para quebrar. Testar os dois manches e D-pad para girar, soltar A antes do vermelho, rebater e voltar a atacar. Verificar combo e partículas; provocar uma derrota e reiniciar. Pausar com Start, retornar com B e confirmar que A precisa ser solto. Abrir a confirmação com Select, cancelar com B, depois sair com Select/A e reabrir para conferir o save. Completar as torres, confirmar desbloqueios e retorno ao Ports.
