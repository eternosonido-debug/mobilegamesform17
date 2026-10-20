# Boca 1.4 — Cidade aberta

Mapa de 3.200 × 2.000 unidades (a praça anterior usava uma tela de 480 × 272), câmera acompanhando o jogador, afastamento gradual da câmera conforme ele cresce e minimapa. 180 objetos com posições aleatórias, com algumas frutas perto do ponto inicial para garantir um começo viável. A posição inicial das bocas rivais também varia.

## Escala e objetivo

60 frutas (raio 3), 40 cones (9), 30 carros (22), 20 caminhões (40), 18 casas (70) e 12 edifícios (110). Os números são raios no mundo, não pixels fixos da tela: a câmera muda de escala. É necessário crescer para absorver cada categoria. A massa recebida cresce com o tamanho do objeto; não é apenas uma troca de desenho.

Objetivo: absorver três edifícios em até dez minutos. É possível absorver bocas rivais menores para crescer também. A cidade continua visível durante a rodada e uma nova rodada gera outra distribuição. A geração tenta evitar sobreposição; há um limite de tentativas e um posicionamento alternativo válido para sementes excepcionalmente densas.

## Rivais e recuperação

Três bocas buscam objetos que conseguem absorver. Elas perseguem o jogador quando são pelo menos 25% maiores e ele está perto; fogem quando o jogador se torna maior. Cada rival também cresce ao comer. Bocas próximas do mesmo tamanho não absorvem uma à outra.

Ao ser atingido por uma boca maior, o jogador perde 20% do raio, com um mínimo de 6 para continuar conseguindo absorver frutas. Ele é afastado do rival e fica protegido por **cinco segundos**, piscando durante esse tempo. A contagem aparece no rodapé. Outros contatos não retiram tamanho durante a proteção. A pausa e a ajuda suspendem a simulação, inclusive a contagem de proteção. Não há morte instantânea por contato.

## Controles e instalação

Direcional move; A dá impulso; X abre ajuda; B fecha ajuda; START abre o menu; SELECT salva e sai. Após vencer ou o tempo acabar, X gera outra cidade. O brilho dourado indica objetos que cabem; contorno vermelho indica uma boca rival perigosa. O minimapa mostra jogador, bocas rivais e edifícios.

Atualizar somente `fmg-test/bin/boca-test` no cartão já preparado. O atalho Ports, o Juices Sort e as configurações do menu são mantidos. Não instalar scripts de boot ou configurações de EmulationStation.

## Salvamento e compatibilidade

A cidade usa FMG 2, com espaço para os 180 objetos, suas posições, rivais, câmera derivada da posição e proteção restante. As partidas dos outros seis jogos continuam sendo gravadas no formato FMG 1. O leitor continua aceitando FMG 1.

A antiga praça do Boca não tem os dados da cidade. Ao carregar uma partida antiga do Boca, esta versão inicia uma cidade nova. Antes da instalação, é guardado um backup da pasta do jogo, incluindo a partida antiga. Partidas da cidade não podem ser abertas pelo binário antigo do Boca. A troca não altera o save do Juices Sort.

## Validação

Build local com AddressSanitizer/UndefinedBehaviorSanitizer; testes das regras e salvamentos anteriores, controles de Juices Sort e Boca e salvamento da cidade. Oito cidades concluídas por um controlador simulado, com movimento contínuo, crescimento desde frutas até edifícios e rivais ativos. Testes de contato com boca maior, proteção por cinco segundos, retomada do dano e absorção de rival menor. Conferência do salvamento completo no início e no fim da progressão. Prévias inspecionadas em cinco escalas e na vitória.

Binário ARM gnueabi compilado com a SDL2 copiada deste cartão M17 W12D. Dependências e símbolos inspecionados estaticamente. O teste de desempenho, câmera e conforto dos controles desta versão no M17 permanece pendente para o usuário.
