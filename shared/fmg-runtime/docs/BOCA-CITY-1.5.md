# Boca 1.5 — cidade viva e volume 2.5D

## Objetos retornam

Objetos absorvidos voltam em posições novas, após um intervalo de tempo jogado:

| Categoria | Intervalo |
|---|---:|
| Frutas | 20 segundos |
| Cones | 34 segundos |
| Carros | 48 segundos |
| Caminhões | 62 segundos |
| Casas | 76 segundos |
| Edifícios | 90 segundos |

O retorno vale para objetos consumidos pelo jogador ou por um rival. O posicionamento tenta evitar sobreposição e reaparecimento sobre bocas, com um limite de tentativas e alternativa dentro do mapa. As contagens são salvas e ficam pausadas no menu/ajuda. Partidas da versão 1.4 continuam abrindo; os objetos já ausentes recebem uma contagem ao retomar pela primeira vez.

## Rivais continuam ativos

Sem comida disponível, os rivais escolhem destinos de exploração. Ao fugir perto de uma borda, seguem pela lateral em vez de insistir em sair do mapa. Se o deslocamento ficar bloqueado, escolhem uma direção de recuperação por dois segundos. Um rival totalmente absorvido pelo jogador retorna após 12 segundos, distante quando possível, com cinco segundos de proteção.

Rivais perseguem bocas menores e fogem de bocas maiores, inclusive uns dos outros. Um rival atingido por outro maior perde 20% do raio (mínimo 6), é afastado e pisca durante cinco segundos de proteção; o atacante recebe parte da massa perdida. Rivais protegidos não recebem outro ataque nem são escolhidos para perseguição nesse período. A regra de dano e proteção de cinco segundos do jogador permanece.

## Arte e leitura

Bordas fixas por identidade: jogador ciano; rivais coral, roxo e amarelo. As mesmas cores aparecem no minimapa. O sinal de exclamação sobre uma boca indica que ela é maior e perigosa. O brilho dourado continua indicando objetos que cabem.

Carros, caminhões, casas e edifícios têm faces superiores e laterais, telhados, cabine/para-brisa, janelas e sombras projetadas. Bocas e objetos são ordenados pela posição no chão para melhorar a profundidade. É um cenário 2D com aparência de volume 2.5D, sem dependência de motor 3D ou novos arquivos externos.

## Compatibilidade e instalação

O mapa, objetivo de três edifícios, limite de dez minutos e controles da 1.4 permanecem. O salvamento continua FMG 2; os campos adicionais guardam retorno de objetos, destinos dos rivais, recuperação e proteção. A instalação substitui somente `fmg-test/bin/boca-test` e guarda backup da pasta do jogo. O Juices Sort, os atalhos Ports, as configurações do menu e os saves existentes são preservados.

## Validação

Build local com AddressSanitizer/UndefinedBehaviorSanitizer. Testes de regras e save anteriores, controles de Boca e Juices Sort, retorno de comida, gravação/leitura de contagens, fuga pela borda, exploração sem comida, combate entre rivais, proteção e retorno de rival absorvido. Oito cidades concluídas com movimento contínuo, rivais ativos e reaparecimento de objetos. Prévias das escalas de carros a edifícios inspecionadas. Binário ARM gnueabi compilado com a SDL do cartão; dependências e símbolos verificados estaticamente.

O desempenho e a sensação desta versão no M17 ainda precisam de teste no aparelho. As simulações locais não equivalem a teste físico.
