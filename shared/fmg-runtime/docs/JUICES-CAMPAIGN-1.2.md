# Juices Sort 1.2.1: campanha

24 fases fixas, ordenadas pelo número mínimo de jogadas (4 a 11). Todas as fases têm solução verificada por busca e testes com as regras reais. São três grupos de oito fases: iniciante, intermediário e avançado. Cada vitória abre a seguinte. A última permite escolher uma fase para repetir.

Visual 1.2.1: tubos com contorno discreto, borda arredondada, marcações laterais e dois tons, seguindo a referência do usuário com menor espessura.

O tubo selecionado fica 14 pixels mais alto, sem marcador em V. Na animação, ele se desloca até o destino, inclina a boca sobre a abertura e retorna à posição original. O jato acompanha a boca do tubo.

## Controles

- Direcional: navegar entre tubos ou opções.
- A: selecionar e despejar. Na vitória, avançar à próxima fase.
- B: cancelar seleção ou voltar nas opções.
- X: opções (dica, reiniciar com confirmação, escolher fase, ajuda, continuar). Na vitória, repetir a fase.
- Y: desfazer até 100 jogadas da sessão, inclusive a jogada de vitória.
- START: menu geral; SELECT: salvar e sair.

Uma dica seleciona a origem e aponta o destino: pressione A para executar. A busca tem limite de 12 mil tabuleiros para limitar memória e processamento. Se não encontrar solução nesse limite, o jogo sugere desfazer ou reiniciar; não declara a partida impossível. A busca não usa rede.

## Progresso

O recorde é o menor número de jogadas em cada fase. Três medalhas ao atingir o mínimo verificado; duas até três jogadas acima; uma em qualquer outra vitória. Não há limite de tempo ou vidas. Dicas e desfazer não retiram medalhas. Repetir ou reiniciar não apaga recordes. As fases anteriores ficam disponíveis no seletor.

`game-0.sav` mantém a partida atual. `juices-progress.sav` mantém recordes e desbloqueios. Ambos usam gravação temporária e substituição, no diretório de saves existente. Partidas antigas sem metadados continuam como PARTIDA CLASSICA. Para iniciar a campanha, X → ESCOLHER FASE → A. Uma vitória na partida clássica também oferece a primeira fase. A troca de fase inicia um novo tabuleiro; a partida clássica anterior deixa de ser a partida atual.

## Instalação no cartão que já funcionou

Com o aparelho desligado e o cartão no leitor, copiar apenas `fmg-test/bin/juices-sort-test` do pacote para o mesmo caminho da partição de jogos. Manter o atalho Ports e a pasta de saves existentes. Ejetar com segurança. Abrir Ports → Juices-Sort-Test. Não copiar configurações de EmulationStation, firmware ou scripts de boot.

## Validação desta versão

Build local com AddressSanitizer e UndefinedBehaviorSanitizer; conclusão das 24 fases com dicas legais, conferência do mínimo de jogadas, medalhas, recordes, desbloqueio e gravação/leitura. Teste de interface SDL com eventos de teclado reais: opções, confirmação de reinício, seleção, dica, despejo animado, desfazer, pausa, saída e retomada. Prévias das telas inspecionadas. Binário ARM compilado com a biblioteca SDL2 copiada deste cartão, com dependências e símbolos inspecionados estaticamente.

O usuário testou a versão 1.1 no M17. A versão 1.2 ainda precisa de teste no aparelho, incluindo o tempo de resposta da dica. Nenhum novo teste físico é alegado aqui.

As fases são reproduzíveis com `python3 tools/generate_juice_levels.py`; os testes C++ verificam as soluções independentemente do gerador.
