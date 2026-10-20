# Boca 1.3 — Praça do Bairro

Arte procedural original: piso, vias, sombras, frutas, latas, pedras, caixas, cones, arbustos, bancos, árvores e carros. A boca tem profundidade e contorno. O brilho dourado mostra objetos pequenos o suficiente para absorver. O contador mostra o progresso e o tempo em minutos e segundos.

## Jogabilidade

- Direcional ou analógico: movimento contínuo, com aceleração e desaceleração.
- A: impulso curto de 0,24 segundo, recarga de 1,2 segundo.
- X: ajuda durante a rodada; nova rodada ao terminar.
- B: fechar ajuda.
- START: pausar no menu geral; SELECT: salvar e sair.

Aproxime a boca dos objetos menores. Quando estão perto, a atração os puxa; eles encolhem ao desaparecer e a boca cresce. Objetos grandes só podem ser atraídos depois que a boca crescer o suficiente. Absorva 60 objetos em dois minutos. Para reiniciar no meio da rodada, START → BOCA → X (nova partida).

Salvamentos antigos são mantidos em `fmg-test/saves/game-6.sav`: a nova arte e a nova movimentação também funcionam com a distribuição antiga de objetos. Uma nova rodada usa a praça com objetos distribuídos em posições espaçadas. O formato FMG 1 permanece compatível. A instalação substitui somente `fmg-test/bin/boca-test`, mantendo o Ports existente e o Juices Sort.

## Validação

Build local com AddressSanitizer/UndefinedBehaviorSanitizer; testes de aceleração, desaceleração, impulso e recarga, atração, crescimento, rejeição de objetos grandes e passo de tempo inválido. Vinte arenas concluídas dentro do tempo por um controlador simulado com movimento contínuo. Teste da interface com eventos SDL de movimento, impulso, ajuda e saída com salvamento. Testes existentes de Juices Sort, demais regras e salvamentos aprovados.

Prévia visual inspecionada. Binário ARM gnueabi compilado com a SDL2 copiada do cartão M17 W12D e dependências/símbolos inspecionados estaticamente. O teste desta atualização no aparelho fica pendente para o usuário; a simulação não mede desempenho nem conforto dos controles no M17.
