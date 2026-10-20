# Boca 1.8 — cidade em 3/4 com lotes e ruas

## Um conjunto de arte

O atlas v2 reúne nove sprites: compacto azul, sedã vermelho, táxi amarelo, SUV verde, ambulância, caminhão dos bombeiros, caminhão de carga, casa e prédio. Todos os veículos e construções usam esse conjunto na execução normal. A mistura de variantes novas com desenhos procedurais antigos foi removida. Os arquivos do atlas anterior deixam de fazer parte da entrega atual.

Veículos podem aparecer alinhados a qualquer um dos dois eixos das ruas, espelhando o sprite quando necessário. Casas e prédios também orientam a fachada conforme a posição do lote. Isso fornece duas orientações em 3/4, sem alegar quatro vistas completas de cada modelo. Frutas e cones conservam suas formas pequenas existentes.

## Cidade e ocupação

A ilha, o mar, a praia, o asfalto e as calçadas usam projeção em 3/4, com eixos diagonais. A ordenação por profundidade considera os dois eixos do chão. A boca permanece centralizada. O direcional continua seguindo as direções da tela; a conversão para o mundo inclinado é feita no controle, não exigindo navegar mentalmente em eixos diagonais.

O asfalto tem 112 unidades de largura e a calçada termina em 124. Veículos são posicionados nas faixas e devem caber inteiros na largura da via. Caminhões usam a faixa central. Casas e prédios são posicionados dentro dos lotes, com recuo suficiente para que a base não ocupe o asfalto. Objetos grandes são distribuídos primeiro; a busca tenta evitar sobreposição e mantém obrigatoriamente a zona correta mesmo no posicionamento alternativo.

A mesma regra vale ao gerar a ilha e ao reaparecer um objeto. Veículos e construções ficam ancorados na posição até serem absorvidos; a animação de desaparecer é visual. A atração de frutas e cones permanece. A boca pode atravessar as ruas e os lotes para alcançar os objetos.

## Salvamento e instalação

Partidas anteriores da cidade são reorganizadas uma vez: os objetos ainda existentes são levados às zonas corretas, mantendo tamanho da boca, pontuação, edifícios já absorvidos, estado dos rivais e contagens de reaparecimento. O formato continua FMG 2. O Juices Sort não muda.

Instalar os dois arquivos juntos, após backup de fmg-test:

- `fmg-test/bin/boca-test`
- `fmg-test/assets/boca-atlas-v2.bmp`

O atlas antigo `fmg-test/assets/boca-atlas.bmp` pode ser removido depois do backup. Não mudar os atalhos Ports, controles ou configurações do menu. Preservar os arquivos de save. A atualização preparada usa cópia sem metadados extras e remove apenas arquivos `._` confirmados do aplicativo, evitando duplicados na lista.

## Validação

Cinco suítes locais com AddressSanitizer/UndefinedBehaviorSanitizer: regras/saves, Juices Sort, controles do Boca, cidade/rivais e atlas. Oito partidas simuladas concluídas com rivais e objetos reaparecendo. Testes verificam todos os veículos no asfalto e construções nos lotes na geração, no reaparecimento e na migração; as duas orientações de ruas; preservação do progresso; e existência de sprite para todas as variantes de veículos e construções. O teste de controle verifica que pressionar direita continua movendo para a direita da tela. Prévias do renderizador SDL inspecionadas.

O binário ARM gnueabi foi compilado com a SDL do cartão M17 W12D e inspecionado estaticamente. O desempenho e a leitura da cidade em 3/4 no aparelho ainda dependem do teste do usuário. Não houve novo teste físico feito pelo assistente.
