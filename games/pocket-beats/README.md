# Pocket Beats — 0.2

Groovebox original offline em C++11/SDL2. Revisão 0.2 nativa compilada para ARM; **atualização ainda não executada no M17 físico**. O usuário testou a 0.1 e relatou interface difícil e possíveis travadinhas.

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
| Abrir/fechar painel de ajustes | Y |
| Selecionar ajuste no painel | Cima/baixo |
| Alterar ajuste destacado | Esquerda/direita |
| Escolher pista na grade | Cima/baixo |
| Iniciar/parar reprodução | Start |
| Salvar e sair para Ports | Select |
| Reservado, sem ação nesta groovebox | X |

Na grade, esquerda/direita sempre escolhem passos e cima/baixo escolhem pistas. Y abre o painel para a pista selecionada; cima/baixo escolhem TEMPO, LOOP, VOLUME, MUTE, SOLO, PAD NOTE ou PATTERN, e esquerda/direita alteram o valor. B ou Y fecham o painel sem parar a reprodução. Os três beats também são acessíveis diretamente no menu inicial, preservando as edições salvas.

Alterações são quantizadas à grade. Reduzir a sequência preserva os passos ocultos. Start reinicia pelo primeiro passo; B na grade para e abre o menu. O salvamento automático ocorre enquanto parado, após edição; Select e a saída normal também salvam. Assim a escrita no SD não compete deliberadamente com a interface durante reprodução. O áudio usa callback independente e banco de sons próprios preparado ao iniciar, sem recalcular ondas a cada amostra durante a reprodução.

Teclado: setas, Z/espaço (A), X (B), C (X), V (Y), Enter (Start), Escape/Shift direito (Select). Compatível com os ajustes FMG_KEY_*, FMG_BUTTON_* e direcionais do controls.sh já existente. Os ombros não são necessários nesta versão. Os dois manches foram verificados com joystick virtual SDL; os eventos físicos do aparelho ainda precisam ser confirmados.

## Sons e limites desta versão

Bateria sintetiza bumbo, caixa e chimbal. NOTE usa 1=bumbo, 2=caixa, 3=chimbal, 4=bumbo+chimbal, 5=caixa+chimbal; 6–8 são variantes equivalentes de chimbal nesta primeira versão. As demais pistas usam oito alturas de uma escala pentatônica. A guitarra é uma voz harmônica percussiva original, não uma gravação realista.

É uma ferramenta musical livre: não há vitória, derrota, pontuação ou campanha. Os três cenários reproduzíveis são os beats iniciais, e o objetivo é criar, ouvir e salvar uma composição. Não há tamanhos independentes por instrumento, gravação por microfone, exportação WAV no menu, edição de timbres ou encadeamento automático de padrões. Sem dispositivo de áudio, a interface indica a ausência de som e o marcador de reprodução não avança.

Veja [BUILD.md](BUILD.md) para compilar e instalar de forma aditiva, e [VALIDATION.json](VALIDATION.json) para o registro de testes. Nenhum cartão foi alterado nesta entrega.

## Correções da revisão 0.2

Removida a dependência da fila de áudio no ciclo de desenho. O callback faz mixagem de um banco próprio de cerca de 1,9 MB, preparado antes de abrir o áudio. Teste com interface bloqueada confirmou que o transporte e a geração continuam. Isto elimina uma fragilidade da 0.1; não comprova que todas as causas de travadinhas no aparelho foram eliminadas.
