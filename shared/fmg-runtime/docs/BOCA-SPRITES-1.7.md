# Boca 1.7 — primeiros sprites detalhados

Cinco assets aprovados foram adaptados para sprites transparentes e integrados ao renderizador: carro azul, ambulância, caminhão dos bombeiros, casa e prédio. A prévia `sprites-no-jogo-v1.png` foi renderizada pelo mesmo código SDL usado no jogo. As variantes de veículos ainda sem sprite próprio continuam usando a arte procedural existente.

Os sprites usam redução com filtragem linear, dimensões e ponto de contato consistentes, respeitando a escala, a profundidade e a animação de absorção. O atlas preserva transparência e não inclui o fundo da prancha de referência. As regras, câmera, mapa e salvamentos permanecem iguais aos da 1.6.

## Arquivos e instalação

A atualização precisa dos DOIS arquivos:

- `fmg-test/bin/boca-test`
- `fmg-test/assets/boca-atlas.bmp`

Preservar o Ports, controles, saves e Juices Sort. Guardar backup de `fmg-test` antes da cópia. O carregador procura `../assets` em relação ao binário, ou usa `FMG_ASSET_DIR` quando definido. Há uma alternativa explícita para o caminho deste cartão M17. Se o atlas não estiver disponível, a arte procedural continua funcionando.

O atlas usa aproximadamente 6 MiB de pixels na textura e é carregado uma vez por execução; a superfície temporária é liberada após criar a textura. PNG fonte e BMP V4 RGBA ficam em `assets/`. Não é necessário instalar SDL_image ou outra biblioteca no aparelho. CMake instala a textura em `assets/` ao lado de `bin/`.

## Validação

Cinco suítes locais aprovadas com AddressSanitizer/UndefinedBehaviorSanitizer: regras/salvamentos, controles do Juices Sort, controles do Boca, cidade/rivais e atlas. A suíte nova carrega o BMP, confere transparência, renderiza os cinco sprites e verifica liberação/recarregamento da textura. A prévia foi inspecionada. O binário ARM foi recompilado e suas dependências/símbolos foram verificados com a SDL do cartão.

Esta entrega prepara código e pacote; não afirma instalação no cartão nem teste físico dos sprites no M17. A memória, desempenho e legibilidade no aparelho ainda precisam ser conferidos.
