# Pocket Beats — 0.4

Groovebox original offline para M17 Linux, em C++11/SDL2. A revisão 0.4 está implementada e compilada para ARM; **teste desta revisão no M17 físico pendente**. O usuário testou a 0.1 e relatou interface difícil e possíveis travadinhas; as correções posteriores não foram confirmadas por nós no hardware.

## Começar sem configurar

Escolha um dos três beats no menu e aperte A. Start toca/para. Use qualquer manche ou D-pad para escolher um quadradinho e A para marcar: o som toca imediatamente. Os beats prontos são Night Drive (104 BPM), Sunny Pop (120 BPM) e Broken Beat (96 BPM); as edições ficam em três espaços separados.

São quatro pistas musicais: bateria, baixo, guitarra e sintetizador. A bateria aparece em três linhas — **bumbo, caixa, chimbal** — para combinar os sons com clareza, inclusive os três no mesmo passo. Cada som de bateria tem sua própria voz: o chimbal não interrompe o bumbo.

As quatro pistas compartilham o tamanho do padrão: **4, 8, 16 ou 32 passos**, inicial 16, com oito passos por página. Quatro passos correspondem a um tempo. Diminuir o padrão preserva os passos escondidos.

## Controles diretos

| Controle | Na grade |
|---|---|
| D-pad / qualquer manche | Escolher passo e instrumento; inclui as três linhas da bateria |
| A | Marcar/desmarcar passo; ao adicionar, ouvir imediatamente |
| L2 / R2 | Descer/subir nota; na bateria, escolher bumbo/caixa/chimbal e ouvir |
| Start | Tocar/parar |
| Y | Abrir seis ações visíveis |
| B | Parar e abrir o menu; voltar em painéis |
| Select | Salvar e sair para Ports |
| X | Reservado, sem ação |

Notas melódicas aparecem por nome (LA, DO, RE, MI, SOL). A nota selecionada mostra a oitava no rodapé. L2/R2 percorrem a escala pentatônica, sem transposição cromática; nos extremos a nota se mantém. Um passo vazio continua vazio: A adiciona a nota. Enquanto parado, selecionar outro passo também permite ouvir seu som.

Teclado: setas, Z/espaço (A), X (B), C (X), V (Y), Enter (Start), Escape/Shift direito (Select), Q/E (L2/R2). Ajustes FMG_KEY_*, FMG_BUTTON_* e direcionais existentes continuam suportados.

## Seis ações no Y

- **TIMBRE:** A alterna Clássico, Brilho e Suave para a pista selecionada, com prévia imediata. São três variações próprias por instrumento, sem custo, downloads ou samples comerciais.
- **COPIAR:** copia o trecho visível de até oito passos, nas quatro pistas.
- **COLAR:** cola na página atual, inclusive em outro padrão. Respeita o tamanho ativo e pode ser desfeito.
- **DESFAZER:** volta uma ação, com histórico de até 32 edições durante a sessão. Funciona para notas, timbres, colagens, ajustes e sequência. O histórico e a área de transferência não são salvos entre execuções.
- **MUSICA:** organiza os três padrões em uma sequência de até oito posições.
- **AJUSTES:** concentra tempo, tamanho, volume, mute, solo, nota, padrão e configuração de L2/R2. A grade continua com navegação direta.

Os direcionais escolhem o cartão e A executa; B volta à grade. TIMBRE fica aberto para comparar os sons. Os demais ajustes ficam fora do caminho principal.

## Juntar padrões em música

Na tela MUSICA, esquerda/direita escolhem uma posição. A alterna o beat dessa posição; no bloco +, A acrescenta o padrão que estava sendo editado. Y remove a posição, mantendo pelo menos uma. Start inicia a sequência (ou para quando ela já está tocando). Cima/baixo alternam modo padrão/música; B volta. A posição tocando fica destacada.

A sequência se repete; a passagem ocorre ao final do padrão, sem reiniciar o dispositivo de áudio. Cada padrão conserva seu tempo e tamanho, sempre iguais para todos os instrumentos daquele padrão. O editor não muda sozinho de página enquanto você mexe: se estiver editando outro padrão durante uma música, a grade não mostra um marcador enganoso. O BPM no cabeçalho acompanha o padrão tocado.

## L2/R2 no M17

O mapeamento anteriormente recuperado da unidade não identifica L2/R2. Controles SDL com gatilhos já mapeados funcionam diretamente. Caso não respondam, abra **Y → AJUSTES → MAPEAR L2 R2 → A**, pressione L2 e depois R2. B cancela. O aprendizado aceita botão digital ou tecla; gatilhos analógicos usam o mapeamento SDL padrão. A/B/X/Y/Start/Select ficam protegidos contra atribuição acidental.

A configuração fica em `pocket-beats-controls.cfg`, junto ao save; não altera controls.sh nem outros jogos. Há também FMG_KEY_L2/R2 e FMG_BUTTON_L2/R2. Solte e aperte novamente para cada alteração. Aprendizado, persistência e cancelamento foram testados com joystick virtual; os gatilhos físicos ainda precisam de confirmação.

## Salvamento e som

Salva automaticamente enquanto parado após edições, e ao sair normalmente. Grava três padrões, notas, sons de bateria independentes, timbres, volumes, mute/solo, sequência, modo e posição de edição. Retoma parado. Saves antigos POCKETBEATS1 são migrados preservando o significado dos sons de bateria. O formato novo é POCKETBEATS2; **faça backup antes de voltar a um executável antigo**, que não lê esse formato.

O áudio roda por callback independente da tela e da escrita no SD. O banco original de três timbres por instrumento é preparado na inicialização (~5,7 MB de PCM); durante a reprodução há apenas mixagem. Bumbo, caixa e chimbal têm vozes separadas, além das três pistas melódicas. A prévia usa vozes próprias e não inicia o transporte. Timbres sintéticos, sem promessa de guitarra acústica realista.

Sem dispositivo de áudio, a interface informa a ausência de som. Ferramenta livre, sem pontuação ou vitória/derrota. Não há exportação WAV no menu, gravação de microfone, tamanho por instrumento, swing, velocity por passo ou edição detalhada de síntese nesta revisão. [REFERENCIAS.md](REFERENCIAS.md) registra projetos externos pesquisados; nenhum código deles foi incorporado.

Veja [BUILD.md](BUILD.md) e [VALIDATION.json](VALIDATION.json). **Nenhum cartão foi alterado para instalar a revisão 0.4.**
