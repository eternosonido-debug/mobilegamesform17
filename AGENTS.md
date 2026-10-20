# AGENTS.md — Mobile Games for M17

Você é agente de engenharia encarregado de implementar jogos **originais** para o portátil M17 com Linux. Leia README.md e o README.md + CODEX.md da pasta específica antes de trabalhar.

## Limites
- Não modificar, apagar ou substituir os cinco jogos anteriores que o usuário afirma já rodarem no M17. Fontes e identidade do quinto ainda precisam ser recuperados e confirmados.
- Não prometer binário funcional sem compilar e validar em hardware. HTML é protótipo, não porte nativo.
- Alvo de tela informado: 480x272. Canvas lógico sugerido: 240x136 escalado 2x por nearest-neighbor. Conferir no aparelho.
- Preferir SDL2/C++ ou C **após** inspecionar o stack dos jogos existentes; não inventar toolchain. Se não houver ambiente de compilação, deixar Makefile claro, indicar dependências e registrar testes pendentes.
- Entrada: D-pad, A/B/X/Y, Start/Select e ombros, somente se efetivamente detectados; criar uma camada de input configurável e fallback teclado no desktop.
- Jogabilidade local/offline; persistência local; evitar dependência de rede.
- Arte própria em pixel art com referência estética 16-bit, som original/licenciado, sem copiar sprites, personagens, interface, marcas ou conteúdo de aplicativos comerciais. Os nomes da curadoria são referências de mecânica.
- Compartilhar recursos genéricos em shared/, sem acoplar os jogos uns aos outros.
- Não alterar Nexo; não introduzir sua infraestrutura.

## Entrega mínima de cada jogo
1. Projeto isolado em games/<slug>/ com código legível, arquivos de build e documentação.
2. Um loop completo: iniciar, jogar, vitória/derrota ou objetivo concluído, reiniciar, pausar e sair.
3. Controles documentados, feedback visual/sonoro, desempenho adequado e sem travamento.
4. Fases reproduzíveis e ao menos três casos de teste das regras; testes automatizados da lógica quando viáveis.
5. Instruções exatas para compilar e instalar no M17, com paths validados antes de escrever no SD.
6. Teste primeiro local e depois no hardware. Registrar FPS, mapeamento de botões e qualquer limitação real. Não alegar teste não feito.
7. Não começar identidade final ou capas dos outros títulos antes da aprovação de sua jogabilidade.

## Fluxo de implementação
Inspecionar código existente → planejar riscos → implementar regras puras e testes → integrar SDL2 → validar teclado e controles → empacotar → teste no aparelho → documentação/arte final. Evitar alterações fora da pasta do jogo, exceto shared/ com justificativa.
