# Pocket Beats

Groovebox original offline em C++11/SDL2. Primeira versão nativa implementada; compilada para ARM, **ainda não executada no M17 físico**.

## O que está disponível

- Quatro pistas sonoras: bateria, baixo, guitarra sintetizada e sintetizador. Todos os sons são gerados pelo código; não há samples comerciais ou pistas mudas de preenchimento.
- Sequência global de **4, 8, 16 ou 32 passos**, inicialmente 16. Todas as pistas compartilham o mesmo tamanho. Oito passos por página, posição de execução destacada.
- Três beats próprios prontos e editáveis: **Night Drive** (104 BPM, espaçado), **Sunny Pop** (120 BPM, melodia mais ativa), **Broken Beat** (96 BPM, síncopes). Cada preset ocupa um dos três espaços de padrões e pode ser restaurado pelo menu.
- Tempo de 40 a 240 BPM, volume/mute/solo por pista e edição de nota por passo. Cada passo representa uma semicolcheia: quatro passos por tempo.
- Salvamento automático e ao sair: três padrões, notas, volumes, mute/solo, tempo, tamanho e posição de edição. Retoma parado para evitar áudio inesperado.
- Menu, tutorial, reprodução/parada, pausa, restauração do beat inicial, limpeza do padrão e saída segura.

## Controles

| Ação | Controle |
|---|---|
| Navegar entre passos e pistas | D-pad ou qualquer um dos dois manches |
| Ligar/desligar passo; confirmar menu | A |
| Parar e abrir menu; voltar | B |
| Extra: percorrer PAD, TEMPO, LOOP, VOLUME, MUTE, SOLO, NOTE, PATTERN | Y |
| Alterar ajuste destacado | Esquerda/direita |
| Escolher pista mesmo durante ajustes | Cima/baixo |
| Iniciar/parar reprodução | Start |
| Salvar e sair para Ports | Select |
| Reservado, sem ação nesta groovebox | X |

Na opção PAD, esquerda/direita navegam a grade. Em PATTERN, escolhem um dos três espaços. Alterações são quantizadas à grade; a reprodução mistura os quatro instrumentos sincronizados. Reduzir a sequência preserva os passos ocultos, que reaparecem ao aumentar novamente. Trocar de padrão durante a reprodução usa o próximo limite de passo; Start reinicia pelo primeiro passo.

Teclado: setas, Z/espaço (A), X (B), C (X), V (Y), Enter (Start), Escape/Shift direito (Select). Compatível com os ajustes FMG_KEY_*, FMG_BUTTON_* e direcionais do controls.sh já existente. Os ombros não são necessários nesta versão. Os dois manches foram verificados com joystick virtual SDL; os eventos físicos do aparelho ainda precisam ser confirmados.

## Sons e limites desta versão

Bateria sintetiza bumbo, caixa e chimbal. NOTE usa 1=bumbo, 2=caixa, 3=chimbal, 4=bumbo+chimbal, 5=caixa+chimbal; 6–8 são variantes equivalentes de chimbal nesta primeira versão. As demais pistas usam oito alturas de uma escala pentatônica. A guitarra é uma voz harmônica percussiva original, não uma gravação realista.

É uma ferramenta musical livre: não há vitória, derrota, pontuação ou campanha. Os três cenários reproduzíveis são os beats iniciais, e o objetivo é criar, ouvir e salvar uma composição. Não há tamanhos independentes por instrumento, gravação por microfone, exportação WAV no menu, edição de timbres ou encadeamento automático de padrões. Sem dispositivo de áudio, a interface indica a ausência de som e o marcador de reprodução não avança.

Veja [BUILD.md](BUILD.md) para compilar e instalar de forma aditiva, e [VALIDATION.json](VALIDATION.json) para o registro de testes. Nenhum cartão foi alterado nesta entrega.
