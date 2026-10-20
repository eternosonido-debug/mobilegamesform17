# Mahjong 1.13

**Controles atuais:** a versão 1.14 passa dicas, ajuda e desfazer para **Y → Extras**, reservando X para ataques. Consulte [CONTROLS.md](CONTROLS.md) para o padrão vigente; os controles descritos abaixo registram a versão de lançamento.

Versão visual renovada do Mahjong de pares do projeto: 32 peças em quatro fileiras de oito, sem camadas sobrepostas. Cada símbolo tem duas peças. Só é possível selecionar uma peça com pelo menos um lado horizontal livre; retire todos os 16 pares para concluir a mesa.

As peças têm face de marfim, lateral com relevo e símbolos desenhados de círculos, bambus e ventos. As bloqueadas ficam mais escuras. O cursor destaca a peça atual, e a primeira peça do par fica elevada. Ao acertar, as duas peças se aproximam, sobem e desaparecem com partículas. Tentar uma peça bloqueada ou combinar símbolos diferentes mostra feedback breve.

## Controles

- Direcional: mover o cursor.
- A: selecionar e combinar duas peças iguais e livres.
- B: cancelar a seleção.
- X: mostrar e selecionar um par livre; A confirma a retirada.
- Y: desfazer a última retirada.
- Start: menu geral; X no menu inicia uma mesa nova.
- Select: salvar e sair para Ports.
- Ao concluir: A ou X abre uma mesa nova; Y desfaz a última retirada.

A mesa informa quantos pares estão disponíveis. Uma sequência diferente da solução gerada pode deixar a mesa sem pares livres; nesse caso, desfaça as retiradas ou inicie outra pelo menu. As mesas são geradas com uma sequência de remoção válida.

O save continua em `fmg-test/saves/game-5.sav`, com o formato já existente. A posição das peças e a animação de retirada são preservadas. Não há necessidade de substituir saves antigos.

## Instalação

No cartão M17 já configurado, copie `fmg-test/bin/mahjong-test` e `.config/emuelec/ports/Mahjong-Test.sh` para os mesmos caminhos. O launcher reaproveita o arquivo `fmg-test/controls.sh` e o runtime SDL que já funcionam no aparelho. Abra **Ports → Mahjong-Test**. Não altera a configuração do EmulationStation.

O pacote conjunto inclui também o novo executável `color-loop-test`: mantém as melhorias de movimento da versão 1.12 e reforça a seleção com elevação, seta, contorno dourado e redução de opacidade dos outros coletores. Não é necessário alterar o launcher existente de Color Loop.

## Validação

Doze grupos de testes locais passaram, incluindo as regras dos outros jogos. O Mahjong foi concluído seguindo a sequência gerada em 60 mesas distintas, com verificação das animações, dicas, peças bloqueadas, desfazer e save/resume. Um teste com eventos SDL reais valida dica, retirada do par, desfazer e saída com gravação.

O executável foi compilado para ARM gnueabi soft-float Cortex-A9, com verificação estática contra as bibliotecas extraídas do M17. Não foi testado no hardware nesta versão.
