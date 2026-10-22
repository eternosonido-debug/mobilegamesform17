# Validação — 0.1.0, 22/10/2026

- Build nativo: Apple Clang/C++11, SDL2, avisos `-Wall -Wextra -pedantic`, AddressSanitizer e UndefinedBehaviorSanitizer; sem avisos ou diagnósticos nos testes.
- Regressão completa da coleção: **30/30 grupos aprovados**, 43,08 s.
- Quatro grupos próprios aprovados: regras/save/soluções, desenho SDL sem tela, controles e fluxo de vitória/desbloqueio.
- Geometria: abertura angular incluindo passagem por 0/360 graus, cruzamentos sólidos bloqueados, abertura no outro anel liberando conexão, pino alinhado, liberação até o objetivo completo e ações inválidas sem alterar o estado.
- Quatro soluções próprias reproduzidas integralmente: 8, 11, 23 e 24 giros. Verificação construtiva, sem alegar otimalidade.
- Save: gravação e retomada dos ângulos/som; rejeição de dados inválidos sem modificar o estado corrente; desfazer restaura ângulos e giros.
- Controles: os dois manches de um joystick SDL virtual selecionam peças diferentes, A/B giram em sentidos opostos, Y desfaz, Start pausa, B retorna e Select sai salvando.
- Fluxo SDL: sequência de botões resolve a primeira fase, mostra vitória, avança para a próxima e salva o desbloqueio.
- Revisão visual: captura real do renderer em 480×272, com pinos dourados visíveis, seleção e legenda. Texturas próprias em memória, sem assets externos ou física pesada.
- Cross-compile: Zig, ARM gnueabi soft-float Cortex-A9, interpreter `/lib/ld-linux.so.3`, SDL2 do runtime ativo recuperado da unidade W12D RK3168.
- Inspeção estática: 541 imports fortes, nenhuma falha; dependências e símbolos registrados em `arm-compatibility.json`; não equivale a execução do binário.

**Execução no M17 físico pendente.** Não foram medidos FPS, latência ou qualidade do áudio no aparelho. Nenhum arquivo de cartão, boot ou outro jogo foi alterado nesta entrega.

## Roteiro no M17

Abrir Ports → Rotate-Rings-Test. Confirmar tutorial, selecionar pelos dois manches e D-pad, girar com A/B e desfazer com Y. Verificar pino e abertura, saída de anel e troca automática da seleção. Pausar com Start e retornar com B. Sair com Select e reabrir para conferir o save. Completar fases para validar desbloqueio e conclusão. Conferir áudio, legibilidade, fluidez e retorno ao Ports antes de considerar a versão validada para console.
