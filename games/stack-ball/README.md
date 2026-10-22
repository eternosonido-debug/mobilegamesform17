# Tower Break 0.2.0

Jogo nativo C++11/SDL2 de descer uma torre quebrando plataformas, com três torres originais. A pasta `stack-ball` é histórica; o executável, o save e a entrada em Ports usam **Tower Break**. Arte, sons e níveis próprios, sem copiar código/assets dos candidatos pesquisados.

## Como jogar

Segure **A** para acelerar e quebrar os trechos coloridos. Solte A para frear o ataque e rebater sobre a plataforma, inclusive em segmentos perigosos. Atacar um trecho escuro com listras vermelhas quebra a bola e termina a tentativa. Soltar A protege na próxima colisão; não congela a rotação nem suspende a bola no ar.

A torre gira automaticamente. Os direcionais permitem ajustar sua rotação para a esquerda/direita. A bola fica na frente da torre; o painel SEGURO/PERIGO informa o segmento atualmente sob ela. O giro continua enquanto você espera, e todas as plataformas possuem setores seguros. Não há tempo limite, vidas, rede ou compras.

Cada plataforma quebrada aumenta o combo; rebater sem atacar zera o combo. Os pontos de cada quebra são `10 × combo`. A velocidade vertical é limitada e cada quebra restabelece a distância até a próxima plataforma, evitando atravessar a pilha inteira em poucos frames. Chegar à base vence e desbloqueia a próxima torre; a terceira encerra a campanha.

| Torre | Plataformas | Setores perigosos em 12 | Rotação automática |
|---|---:|---:|---:|
| 1 | 640 | 2 | 72 graus/s |
| 2 | 680 | 3 | 108 graus/s |
| 3 | 720 | 4 | 144 graus/s |

A conclusão exige pelo menos 3 minutos de jogo ativo. Não há espera artificial na base: são torres mais longas, com limite vertical de 700 milésimos de pixel por passo e distância de 24.000 entre quebras. Mesmo supondo velocidade máxima instantânea e nenhum perigo, os limites inferiores são 186,59 s, 198,26 s e 209,92 s. Menus e pausas não avançam a física nem o relógio mostrado na tela. As simulações com ataques seguros terminaram em 274,13 s, 297,38 s e 387,75 s; não são tempos mínimos nem medidas em hardware.

Há um marco a cada 80 andares. Na derrota, A retoma do último marco, mantendo o tempo ativo acumulado; reiniciar pelo menu começa a torre do zero. Saves da versão anterior mantêm andares já quebrados e desbloqueios, mas passam a usar as torres longas; a velocidade antiga é limitada ao novo máximo. O formato novo é TOWER2 e lê também TOWER1.

As posições dos perigos mudam entre plataformas por uma regra fixa versionada. As três torres foram concluídas na simulação das regras com uma sequência de ataques seguros. Isso prova alcance da base no modelo, sem medir a dificuldade percebida nem substituir teste humano.

## Controles

- **A mantido:** atacar/descer. Soltar: rebater sem quebrar a plataforma.
- **D-pad ou qualquer um dos dois manches, esquerda/direita:** ajustar rotação. Cima/baixo navegam nos menus.
- **B ou Start:** pausar. B retorna da pausa.
- **Select:** abrir confirmação para salvar e sair ao Ports; A confirma, B cancela.
- **X/Y:** sem função nesta versão.

Teclado: Z/espaço=A, setas para os direcionais, X=B, Enter=Start e Escape=Select. Há suporte às variáveis `FMG_KEY_*`, `FMG_BUTTON_*` e aos eixos configuráveis da coleção. A mantido é lido tanto pelo teclado quanto por controlador SDL ou joystick bruto; as configurações alternativas foram testadas no computador. O launcher carrega `fmg-test/controls.sh` da unidade.

O jogo exige soltar A depois de fechar o tutorial, retomar ou cancelar a saída, evitando ataque acidental com o botão usado no menu. Perder o foco pausa automaticamente. O menu oferece continuar, reiniciar, trocar entre torres desbloqueadas, silenciar e sair. Na derrota, A reinicia e B abre o menu; na vitória, A avança ou volta ao menu ao concluir a campanha.

## Implementação e save

A física avança em passos inteiros de 1/120 s, independentemente do desenho. A acumulação de tempo por frame é limitada para não tentar recuperar pausas longas de sistema. O desenho usa perspectiva 2.5D com elipses, cilindro e setores coloridos, sem engine 3D. Uma tabela angular é calculada uma vez e usada para desenhar as plataformas sem trigonometria por pixel a cada frame. Partículas e sons dão retorno às quebras.

`tower-break.save` guarda torre, desbloqueios, som, plataforma, ângulo, altura, velocidade, combo, pontos, tempo lógico e derrota. A escrita usa arquivo temporário e rename. Há save nos menus, na saída e no resultado; progresso de quebras também é salvo ao soltar A e passar o intervalo de autosave. O estado de A não é persistido: ao retomar, é preciso soltá-lo antes de atacar. Saves inválidos são rejeitados sem alterar outros arquivos. Não lê nem altera saves de outros jogos.

Regras/save em `src/game_logic.hpp`, desenho em `src/renderer.hpp`, entrada em `src/input.hpp` e aplicação em `src/main.cpp`. Componentes genéricos próprios reutilizados sob [LICENSE-FMG](../../LICENSE-FMG). Veja [REFERENCIAS.md](REFERENCIAS.md), [BUILD.md](BUILD.md) e [VALIDACAO.md](VALIDACAO.md).

**Em desenvolvimento para console:** executável ARM compilado e inspecionado; ainda não executado no M17 físico. FPS, áudio e controles reais permanecem pendentes. Esta entrega não instalou arquivos no cartão.
