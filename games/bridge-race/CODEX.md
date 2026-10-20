# CODEX — Bridge Builder Race

Leia /AGENTS.md e o README desta pasta antes de codificar.

Crie jogo Linux SDL2 **nativo** e original (não HTML), aproveitando stack dos jogos existentes apenas após inspecioná-lo. Não alterar os cinco jogos anteriores.

**Conceito:** Coletar blocos da própria cor e erguer uma ponte até o objetivo.
**Regras obrigatórias:** Coleta e pilha, consumir blocos automaticamente na ponte, competidor IA, chegada; 3 pistas.
**Controles planejados:** direcionais mover; A ação contextual; Start pausa, saída segura.
**Visual:** Câmera superior simplificada; resolução 480x272 e canvas 240x136 com nearest-neighbor se validado.

Tarefas detalhadas:
1. Modele entidades, grade/câmera e estado do jogo separado do render.
2. Implemente telas menu, tutorial, gameplay, pausa, vitória e derrota quando aplicável.
3. Projete três níveis determinísticos e solucionáveis (dados versionados); explicite objetivos, falha e pontuação.
4. Crie input via SDL com mapeamento configurável e teclado para debug; não exigir mouse/touch.
5. Implemente física/colisões e validação de ações, garantindo estabilidade a 30 ou 60 FPS conforme testes reais.
6. Adicione feedback audiovisual simples e assets autorais leves; não use materiais protegidos.
7. Inclua Makefile, BUILD.md e instruções de instalação sem inventar diretórios nem toolchain.
8. Teste início, fase válida, situação de derrota, vitória, reinício, controles e gravação de progresso.
9. Registre comandos executados, evidências de build e o que não foi testado no hardware.

**Definição de pronto:** compilação nativa comprovada, três fases concluíveis, controles do M17 verificados, instruções reprodutíveis e zero interferência nos títulos anteriores. Até lá status 'em desenvolvimento'.
