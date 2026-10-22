# Cooking 0.1.0 — Confeitaria Colorida

Novo jogo independente do Minichef: uma confeitaria de experimentação livre, inspirada na sequência de preparo das imagens fornecidas. Escolha ingredientes, leve à tigela, misture, preencha quatro formas, asse e personalize cada cupcake. Sem corrida de clientes, monetização, publicidade, toque obrigatório, ingredientes perdidos ou game over. Offline, C++11/SDL2 nativo, 480×272.

## Uma receita, seis momentos

1. **Ingredientes:** baunilha, chocolate ou morango; farinha, açúcar, manteiga, ovos, leite e sabor em torno da tigela. Direcionais selecionam, A pega e verte. A animação leva 1,1 s, ou 1,5 s nos ovos. O ingrediente só entra no final e não pode ser duplicado. Pausar durante a animação congela o preparo; o save retoma a mesma posição.
2. **Mistura:** gire qualquer manche, ou percorra o D-pad em círculo. Sentido horário ou anti-horário. Movimentos adjacentes contam, saltar diretamente para o lado oposto não conta. São 12/16/20 gestos para os três sabores. Não exigir toques A repetidos. Colher e massa giram e a tigela brilha ao completar.
3. **Formas:** escolha uma das quatro, A transfere uma concha de massa. Ela se desloca e derrama antes de a forma ficar cheia. Uma porção por forma, sem desperdício.
4. **Forno:** A coloca a bandeja. Esquerda/direita ajustam a intensidade de 1 a 3; os bolinhos crescem com o tempo. Na intensidade 2, assam por 15/18/21 s. Ao dourar, A retira e abre a decoração. Não é possível retirar crus, nem queimar se demorar; não há limite total ou espera artificial para completar três minutos.
5. **Decoração livre:** seis escolhas de ferramenta numa única tela. Cobertura (sem + seis cores), confeitos (quatro opções), fruta (cinco), carinha (quatro), forminha (seis) e PRONTO. Esquerda/direita escolhem ferramenta; cima/baixo escolhem a opção; A aplica. Y muda entre os quatro doces, B desfaz a última alteração. Não é necessário decorar todos ou combinar um modelo obrigatório.
6. **Foto/álbum:** selecione PRONTO e A. Guarda o conjunto de quatro desenhos, sabor e número da criação no álbum local. A começa outro sabor; B retorna ao menu. O álbum mantém os últimos oito conjuntos, em ordem do mais novo para o mais antigo. Fazer outro conjunto não apaga o preparo enquanto não confirmar a nova receita no menu.

Todos os sabores e enfeites estão disponíveis desde o início. Cada conjunto conserva o total produzido por sabor. Não há fotografias de câmera: a tela final redesenha o prato a partir das escolhas salvas. Regras em `src/game_logic.hpp`; preparação determinística de prévias/testes em `src/demo.hpp` usa as mesmas ações do jogo.

## Controles físicos e teclado

D-pad e **ambos os manches**: seleção/gestos. A principal; B pausa durante preparo ou desfaz durante decoração; Y ajuda durante preparo ou troca o doce na decoração. Start pausa em qualquer etapa. Select confirma salvar e voltar ao menu; no menu, salva e sai para Ports. X/ombros não são necessários.

Teclado: setas, Z/espaço=A, X=B, V=Y, Enter=Start, Escape=Select. Também usa `controls.sh` com `FMG_KEY_*`, `FMG_BUTTON_*`, eixos/zonas mortas genéricos da coleção. Segurar o direcional não repete gestos de mistura. Perder o foco pausa automaticamente. Ajuda de decoração também está no menu de pausa.

No menu, a linha de sabor muda com esquerda/direita; A inicia uma nova receita. Continuar retoma o estado salvo. Álbum abre as oito criações mais recentes. Novas receitas limpam apenas a bancada, preservando álbum e totais.

## Arte, animações e áudio

Cenário original de confeitaria, com cores vivas, céu e colinas arredondadas, utensílios e madeira ilustrada. Gerado pela ferramenta integrada de imagens; [prompt, processamento e proveniência](assets/ART.md). Não utiliza personagens, sprites, símbolos ou música de Mario ou das capturas de referência. As capturas privadas não são redistribuídas. O arquivo de imagem de execução está incluído em `assets/kitchen.bmp`.

Doces, tigela, ingredientes, personagem Pip (um passarinho chef), bandeja e forno são originais desenhados por código. Animações: arco dos ingredientes, inclinação e derramamento, cascas dos ovos, gema, redemoinho, concha, crescimento no forno, vapor/brilhos, mascote e confetes. Doces ficam em cache de textura e só são redesenhados quando seu desenho/sabor muda. Cenário e ingredientes também usam texturas. Efeitos e pequena fanfarra são sintetizados e calculados na inicialização, sem trabalho de síntese durante cada botão. Sem música contínua nesta versão.

## Save

`cooking.save`, COOK1: sabor, etapa, seleção, ingredientes já usados, animação em curso, mistura, formas, forno, quatro desenhos, desfazer, álbum, totais e opção de som. Temporário + rename; salva nas ações relevantes, transições, pausa/saída e a cada cinco segundos ativos. Interrupção abrupta pode perder até cinco segundos; saída normal salva o estado corrente. Save malformado/fora dos limites é rejeitado sem substituir o estado carregado.

Fontes/arte procedural próprios sob [LICENSE-FMG](../../LICENSE-FMG), cenário gerado fornecido com o projeto para uso/adaptação sob os mesmos termos na medida dos direitos aplicáveis. Componentes próprios de Minichef reutilizados, sem modificar esse jogo. [Pesquisa prévia](REFERENCIAS.md), [build/SD](BUILD.md), [validação e pendências](VALIDACAO.md).

**Em desenvolvimento.** Compilado para ARM e testado no computador. Ainda sem execução física, medição de FPS, conforto, qualidade do LCD ou validação dos botões desta versão no M17.
