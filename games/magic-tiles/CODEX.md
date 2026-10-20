# Instrução de execução para Codex — Magic Tiles

**Objetivo:** implementar um jogo ORIGINAL chamado provisoriamente Magic Tiles, baseado nas regras descritas em [README.md](README.md). Leia também /AGENTS.md.

## Dinâmica a implementar (fonte de verdade)
**Loop principal:** Jogo rítmico de notas descendo por pistas sincronizadas à música. 
**Regras e condições:** Mapa de tempo em milissegundos; janelas de acerto; combo; precisão; seleção de dificuldade; calibração de latência.
**Interface e entrada:** A/B/X/Y pistas básicas; ombros acrescentam pistas ou modificadores em modo avançado.
**Estilo gráfico:** Colunas destacadas com notas grandes.
**Restrições específicas:** Usar músicas próprias ou licenciadas; testar taxa de atualização e latência.

## Trabalho requerido
1. Inspecione o repositório e encontre o projeto existente dos cinco jogos antes de escolher dependências. Se não houver código, documente o bloqueio e ofereça implementação SDL2 autocontida, sem alterar projetos antigos.
2. Separe regras puras (`src/game_logic.*`) de renderização (`src/renderer.*`) e entrada (`src/input.*`). Faça um executável nativo para Linux; não entregue só HTML.
3. Modele explicitamente estados MENU, TUTORIAL, PLAYING, PAUSED, WON, LOST (quando aplicável), GAME_OVER; implemente transições controladas.
4. Implemente três fases/níveis reproduzíveis em dados externos ou cabeçalhos, com objetivos claros e progressão. Para puzzle, certifique-se de que cada nível tem solução e que ações inválidas não alteram o estado.
5. Use viewport lógico 240x136 com upscale 2x sem blur para 480x272, salvo conflito verificado com o aparelho. UI deve ser legível a 4,3 polegadas.
6. Configure mapeamento dos botões do M17 via SDL_GameController/SDL_Joystick somente após inspecionar os eventos reais; ofereça controles de teclado no desktop. Inclua retorno/pausa e saída segura.
7. Crie assets originais leves (pixel art, sem copiar jogos comerciais). Se não existirem, use primitivas SDL inicialmente; separe futura produção de capas e menus definitivos.
8. Adicione `Makefile` e `BUILD.md` com compilação para Linux nativo e instruções de cross-compilação SOMENTE se toolchain confirmada. Registre dependências.
9. Adicione testes automatizáveis da lógica para no mínimo: ação válida, ação inválida, vitória/derrota; e teste de salvamento quando aplicável.
10. Documente como rodar localmente, instalar sem sobrescrever os cinco jogos antigos, e como retornar ao menu original do M17. Não invente caminhos do cartão SD.
11. Ao terminar, informe arquivos criados, comando de build, saída real da compilação/testes e pendências de validação em hardware. Se não foi testado no M17, diga explicitamente.

## Critérios de pronto
- Jogo completo de início ao fim em pelo menos 3 fases; controles sem mouse/toque; UI legível; offline; estrutura reutilizável; build reproduzível; testes de regra passando; teste no M17 documentado antes de declarar 'pronto para console'.
- Não alterar nomes/arquivos de outros jogos, nem infraestrutura do Nexo.

## Atenção a direitos autorais
Criar gráficos, sons, níveis e lógica de implementação próprios. A referência inspira o gênero/mecânica; não copiar assets, níveis ou código.
