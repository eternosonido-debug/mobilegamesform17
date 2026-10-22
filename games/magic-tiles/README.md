# Magic Tiles 0.2.0

Jogo rítmico original, offline e nativo C++11/SDL2. Quatro pistas grandes em A/B/X/Y, dez músicas próprias e três dificuldades por música. Áudio sintetizado por amostras próprias, com relógio independente da interface, pausa, treino, ajuste de latência e recordes locais. Sem músicas, charts, personagens ou arte de jogos comerciais.

## Como jogar

Toque o botão da pista quando a nota passar pela linha inferior. Da esquerda para a direita: **A, B, X, Y**. Cada pressão julga uma nota; manter o botão pressionado não repete acertos. Notas duplas aparecem no difícil. Sem notas longas nesta primeira versão.

| Música própria | BPM | Duração completa | Notas fácil / normal / difícil |
|---|---:|---:|---:|
| Brisa Digital | 96 | 3:04,500 | 144 / 288 / 612 |
| Rua Neon | 112 | 3:04,142 | 168 / 336 / 714 |
| Pulso Noturno | 128 | 3:03,875 | 192 / 384 / 816 |
| Mar De Vidro | 88 | 3:04,727 | 132 / 264 / 561 |
| Sol De Cobre | 104 | 3:04,307 | 156 / 312 / 663 |
| Chuva Lilas | 120 | 3:04,000 | 180 / 360 / 765 |
| Orbita Azul | 100 | 3:04,400 | 150 / 300 / 638 |
| Luz Da Serra | 116 | 3:04,068 | 174 / 348 / 740 |
| Vento Solar | 132 | 3:03,818 | 198 / 396 / 842 |
| Amanhecer | 140 | 3:03,714 | 210 / 420 / 893 |

Cada faixa tem contagem inicial de quatro batidas, 180 s de conteúdo musical e 2 s de cauda. A vitória só acontece ao terminar a faixa, com mais de três minutos de relógio de áudio ativo; menus e pausas não contam. É possível perder antes disso. As composições usam motivos próprios, progressão harmônica, baixo, melodia, bumbo, caixa e chimbal sintetizados. Os mesmos eventos melódicos geram a música e os charts, evitando mapas sem relação com o som.

## Julgamento e progresso

- Perfeito: erro até ±55 ms, peso 100.
- Bom: até ±105 ms, peso 75.
- Quase: até ±140 ms, peso 40.
- Nota perdida: reduz 5 de vida e zera combo. Pressionar sem uma nota na janela reduz 2.
- Um acerto recupera 1 de vida. Vida zerada encerra a tentativa; o modo Treino permite continuar sem gravar recorde.

A pontuação é normalizada até 1.000.000; uma execução perfeita atinge esse valor em qualquer mapa. O menu mostra o recorde da música/dificuldade selecionada. As dez músicas estão disponíveis desde o início. A dificuldade muda a quantidade de notas, sem alterar a música.

## Controles e menu

A/B/X/Y ativam as quatro pistas durante a música. Alternativa: esquerda/baixo/cima/direita no D-pad ou em qualquer um dos dois manches. É preciso retornar ao neutro ou mudar de direção para produzir outro toque; o difícil com notas duplas é mais confortável com os botões. No teclado: Z/espaço, X, C e V; setas são a alternativa direcional.

Start pausa. Na pausa, A/Start continua e B volta ao menu. Select volta ao menu durante a partida e sai para Ports quando já estiver no menu. A confirma as opções. O menu usa os direcionais para navegar e oferece música, dificuldade, ajuste de atraso, treino, som e saída. Perder foco pausa automaticamente.

Em **ATRASO**, esquerda/direita ajustam entre −150 e +150 ms em passos de 5 ms; A volta a zero. Valor positivo adianta o relógio de julgamento e a posição visual das notas em relação ao áudio; negativo atrasa. Ajuste ouvindo e tocando no aparelho. O fim da faixa usa o relógio bruto, portanto o ajuste negativo não deixa a partida presa na cauda.

Entrada configurável pelas variáveis `FMG_KEY_*`, `FMG_BUTTON_*` e eixos já usados na coleção, carregados pelo launcher via `controls.sh`. Este MVP possui quatro pistas; expansão por ombros não foi implementada e depende de escopo/mapeamento físico posterior.

## Save e áudio

`magic-tiles.save` registra seleção, dificuldade, atraso, treino, som e os trinta recordes. Saves TILES1 anteriores são importados, mantendo os nove recordes originais; o novo formato TILES2 guarda os 30. A escrita usa arquivo temporário e rename. Uma música interrompida reinicia do começo; não há retomada no meio da faixa. Outros saves e jogos não são alterados.

O callback SDL mistura amostras pré-calculadas e publica a posição de reprodução por operações atômicas. A interface lê esse relógio, com interpolação limitada ao buffer; não agenda música pelo frame de vídeo. A pausa congela o relógio. A latência real de saída ainda depende do driver/aparelho e precisa de calibração; os testes em computador não comprovam a sincronização percebida no M17. O modo sem som mantém o relógio ativo; é necessário um dispositivo de áudio SDL válido para iniciar.

Regras e mapas: `src/game_logic.hpp`; síntese/relógio: `src/audio.hpp`; desenho: `src/renderer.hpp`; aplicação/controles nos demais arquivos. Sons, melodias e código próprios sob [LICENSE-FMG](../../LICENSE-FMG). Veja [pesquisa prévia](REFERENCIAS.md), [BUILD.md](BUILD.md) e [VALIDACAO.md](VALIDACAO.md).

**Compilado para ARM; ainda não executado no M17 físico.** Não foram medidos FPS, latência percebida, qualidade do alto-falante ou conforto de tocar uma música completa no aparelho. Nenhum arquivo foi instalado no SD nesta entrega.
