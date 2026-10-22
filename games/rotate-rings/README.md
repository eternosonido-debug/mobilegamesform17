# Rotate Rings 0.2.0

Puzzle nativo C++11/SDL2 com 20 fases originais. Gire anéis abertos para passar pelos pinos e desencaixar as conexões com outros anéis. Inclui rotação suave, saída animada, efeitos sonoros, tutorial, pausa, desfazer, desbloqueio e save separado. Offline, sem mouse ou toque.

## Regras

Cada giro tem 30 graus, com 12 posições possíveis. Cada anel possui uma abertura de 90 graus e um pino fixo da mesma peça no tabuleiro. O anel sai automaticamente quando a abertura está alinhada com seu pino e os cruzamentos com todos os anéis restantes estão livres. Em cada ponto de cruzamento, basta que a abertura de um dos dois anéis permita a passagem. Portanto, girar um anel também pode ajudar a libertar outro. A seleção passa para um anel restante depois da animação.

O objetivo é soltar todos os anéis. Não há tempo limite nem vidas. Pode desfazer ou reiniciar sem penalidade. A geometria calcula as duas interseções entre circunferências; não simula física 3D nem colisão contínua ao longo da trajetória de saída. Os giros são permitidos para explorar alinhamentos; o teste geométrico decide a liberação. Os pinos usam a cor de cada anel. O anel selecionado fica mais iluminado, com centro da mesma cor, cantos claros e nome na lateral; uma seta branca indica sua abertura. A ordem visual das camadas permanece fixa ao selecionar outra peça. Os centros estão mais afastados, reduzindo as sobreposições, e os arcos possuem contorno escuro.

## Controles

- D-pad ou qualquer um dos dois manches: selecionar o anel na direção indicada.
- **A:** girar no sentido horário; confirmar menus.
- **B:** girar no sentido anti-horário; voltar nos menus.
- **Y:** desfazer um giro, incluindo anéis libertados por ele.
- **Start:** pausar; B volta da pausa para o tabuleiro.
- **Select:** salvar e sair para Ports.
- **X:** sem função nesta versão.

Teclado: setas, Z/espaço=A, X=B, V=Y, Enter=Start, Escape=Select. A entrada respeita as variáveis configuráveis `FMG_KEY_*`, `FMG_BUTTON_*` e eixos dos direcionais já usadas pela coleção. O launcher carrega o `controls.sh` existente. Mapeamento real dos botões no aparelho ainda precisa de teste.

## Fases

| Fase | Anéis | Mínimo de giros verificado |
|---|---:|---:|
| 1 | 2 | 2 |
| 2 | 3 | 4 |
| 3 | 4 | 6 |
| 4 | 4 | 8 |
| 5 | 4 | 9 |
| 6 | 4 | 10 |
| 7 | 4 | 11 |
| 8 | 4 | 12 |
| 9 | 4 | 13 |
| 10 | 4 | 14 |
| 11 | 4 | 15 |
| 12 | 4 | 16 |
| 13 | 4 | 17 |
| 14 | 4 | 18 |
| 15 | 5 | 19 |
| 16 | 5 | 20 |
| 17 | 5 | 21 |
| 18 | 5 | 22 |
| 19 | 5 | 23 |
| 20 | 5 | 24 |

A progressão começa com dois anéis, passa por correntes e circuitos com quatro e termina com cinco anéis. Cada fase exige mais giros na solução mínima que a anterior: 2, 4, 6, 8 e depois de 9 até 24. A busca A* nos testes calcula o mínimo sob as regras discretas do jogo; cada fase também possui uma sequência construtiva reproduzida até a vitória. O algoritmo usa como estimativa admissível os giros mínimos independentes para alinhar os pinos e não roda no aparelho durante a partida. Quantidade mínima de giros é uma medida objetiva de progressão, mas não garante como cada pessoa perceberá a dificuldade. As posições iniciais foram recalibradas; fases 1–4 mantêm sua geometria para importar as partidas anteriores.

Completar uma fase desbloqueia a próxima; o menu identifica a fase atual e permite trocar entre as liberadas, reiniciar, silenciar e sair. A etapa 20 encerra a campanha.

O arquivo `rotate-rings.save` guarda fase, seleção, ângulos, anéis restantes, giros, desbloqueios e som. A gravação usa arquivo temporário e rename. O histórico de desfazer é limitado a 128 ações e existe apenas na sessão atual. Saves das versões 0.1.0 e 0.1.1 são importados com progresso e ângulos preservados; se o maior espaçamento liberar uma conexão, ela é normalizada ao carregar. O formato novo é RINGS3 e guarda os cinco ângulos; os formatos antigos RINGS1/RINGS2, com quatro ângulos, continuam legíveis. Um save inválido é rejeitado e inicia-se uma partida nova. Saves dos outros jogos não são lidos nem modificados.

## Entrega e fontes

- `src/game_logic.hpp`: geometria, fases, regras, verificação de soluções e save.
- `src/renderer.hpp`: texturas próprias geradas uma vez em memória, desenho e animações. Os giros reaproveitam as texturas, evitando redesenhar arcos ponto a ponto a cada frame.
- `src/input.hpp`, `src/audio.hpp`, `src/pixel.hpp`: componentes próprios reaproveitados da coleção, sob [LICENSE-FMG](../../LICENSE-FMG).
- `src/main.cpp`: menu, tutorial, jogo, pausa, vitória e conclusão da campanha.
- `tests/`: regras, mínimo de giros por A*, save, controles com manches virtuais, vitória/desbloqueio pelo fluxo SDL.

Veja [BUILD.md](BUILD.md), [VALIDACAO.md](VALIDACAO.md) e [REFERENCIAS.md](REFERENCIAS.md). Nenhum código, nível ou asset externo foi copiado. **Compilado para ARM e testado no computador; execução física no M17 pendente.** Não instalado no cartão nesta entrega.
