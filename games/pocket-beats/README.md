# Pocket Beats — 0.6

Groovebox original e gratuita, offline, em C++11/SDL2. A revisão 0.6 simplifica a interação: quatro pistas e todos os passos na mesma tela. **Compilada para ARM, ainda sem teste desta revisão no M17 físico.**

## Escolha o espaço, toque a nota, ligue com A

Use D-pad ou qualquer manche para escolher o espaço na pista. Os sete botões abaixo escolhem a nota e deixam ouvir o som imediatamente. **Eles não ligam um espaço vazio. A é o único botão que liga/desliga o espaço**, preservando a nota escolhida quando você o desliga.

| Botão | Baixo, guitarra e synth | Bateria |
|---|---|---|
| L1 | Dó | Bumbo |
| R1 | Ré | Caixa |
| L2 | Mi | Chimbal |
| R2 | Fá | Tom grave |
| X | Sol | Tom médio |
| Y | Lá | Tom agudo |
| B | Si | Prato |

Com o beat parado, você também pode tocar livremente esses botões e ouvir os instrumentos. Em um espaço ligado, escolher outra nota substitui o som daquele passo. Na bateria, os sons têm vozes separadas: um prato não corta o bumbo. Beats antigos podem manter combinações de bateria, identificadas como KIT; escolher um dos sete botões substitui a combinação pelo som escolhido.

- **Start:** toca/para o beat.
- **Select:** abre/fecha o menu simples; a reprodução para ao abrir.
- **A nos menus:** confirma.
- **B nos menus:** volta. Na grade, B é sempre a nota Si/prato.

O rodapé mostra os sete atalhos, o som selecionado e se o passo está ligado. Não há combinações de botões, transposição escondida, páginas ou modo de ajustes na grade.

## Tudo na mesma tela

As quatro pistas são BATERIA, BAIXO, GUITARRA e SYNTH. O tamanho é global: **4, 8, 16 ou 32 passos** para todos os instrumentos. Os passos aparecem juntos nos 480×272 pixels; o cursor nunca troca ou rola a página. Separadores a cada quatro passos ajudam a leitura. Quatro passos correspondem a um tempo.

Em 16 passos, SO abrevia SOL. Em 32, as notas usam A/C/D/E/F/G/B nas células compactas; o nome completo e a oitava aparecem no rodapé da seleção. Sons de bateria usam abreviações nas células e nome completo no rodapé. O tamanho menor preserva os passos escondidos para quando você aumentar novamente.

## Poucas funções no menu

CONTINUAR, TEMPO, TAMANHO, TROCAR BEAT, VARIAR O BEAT, DESFAZER, CONFIGURAR L/R e SALVAR E SAIR.

Tempo (40–240 BPM), tamanho e beat usam esquerda/direita e A confirma. Três beats próprios estão prontos e editáveis: Night Drive (104 BPM), Sunny Pop (120 BPM), Broken Beat (96 BPM).

**VARIAR O BEAT** cria uma combinação nova de bateria, baixo, guitarra e synth, com preenchimentos de tons e notas da escala de Dó maior. Mantém tempo e tamanho. **DESFAZER** recupera a versão anterior, com histórico de até 32 edições na sessão. Assim dá para experimentar sem montar tudo do zero. A geração é local e gratuita, sem serviço de IA ou rede.

Copiar/colar, sequência de música, mute/solo, edição de timbres e painéis avançados foram retirados da interface para priorizar tocar e montar o beat. Os sons são sintetizados pelo próprio código, sem samples comerciais. A mixagem básica usa volumes fixos equilibrados; ao abrir saves antigos, mute/solo e modo de sequência são desativados, evitando instrumentos silenciosos por controles que não existem mais. Notas e timbres antigos continuam disponíveis na importação.

## Configurar L1/R1/L2/R2 no M17

Mapeamentos SDL de ombros/gatilhos funcionam diretamente quando presentes. O mapeamento recuperado desta unidade não identifica todos esses botões. Se necessário: **Select → CONFIGURAR L/R → A**, pressione L1, R1, L2 e R2 na ordem mostrada. B cancela. A configuração fica apenas em `pocket-beats-controls.cfg`; não altera controls.sh nem outros jogos. O aprendizado digital e a persistência foram testados com joystick virtual, mas os botões físicos ainda precisam de confirmação.

Aceita também FMG_KEY_L1/R1/L2/R2 e FMG_BUTTON_L1/R1/L2/R2 explícitos. A captura aceita botões digitais ou teclas; gatilhos analógicos usam o mapeamento SDL padrão. Mapeamentos antigos de L2/R2 são lidos como Mi/Fá. A/B/X/Y/Start/Select são protegidos durante a configuração dos ombros.

Teclado: setas; Z/espaço=A; Q/W/E/R=L1/R1/L2/R2; C=X; V=Y; X=B; Enter=Start; Escape/Shift direito=Select. Os ajustes FMG_KEY_* existentes prevalecem sobre esses padrões.

## Saves e validação

Salva automaticamente enquanto parado depois das edições e ao sair normalmente. Grava os três beats, notas ativas, atribuições de notas em espaços desligados, tempo, tamanho e posição. Retoma parado. Importa POCKETBEATS1 e POCKETBEATS2; grava **POCKETBEATS3**. Faça backup do save antes de usar esta revisão, pois executáveis anteriores não leem o novo formato.

O áudio usa callback independente da tela e da escrita no SD. O banco original é preparado antes de tocar (~10,2 MB de PCM), sem síntese pesada por amostra no callback. Não há compras, rede, anúncios ou assets pagos. Sem dispositivo de áudio, a interface informa a ausência de som. Não há pontuação ou vitória/derrota.

[BUILD.md](BUILD.md) descreve compilação e instalação aditiva; [VALIDATION.json](VALIDATION.json) registra os testes. [REFERENCIAS.md](REFERENCIAS.md) registra a pesquisa de outros programas. Nenhum código deles foi incorporado. **Esta revisão não foi instalada no cartão.**
