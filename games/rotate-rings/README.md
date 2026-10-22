# Rotate Rings 0.1.0

Puzzle nativo C++11/SDL2 com quatro fases originais. Gire anéis abertos para passar pelos pinos e desencaixar as conexões com outros anéis. Inclui rotação suave, saída animada, efeitos sonoros, tutorial, pausa, desfazer, desbloqueio e save separado. Offline, sem mouse ou toque.

## Regras

Cada giro tem 30 graus, com 12 posições possíveis. Cada anel possui uma abertura de 90 graus e um pino fixo da mesma peça no tabuleiro. O anel sai automaticamente quando a abertura está alinhada com seu pino e os cruzamentos com todos os anéis restantes estão livres. Em cada ponto de cruzamento, basta que a abertura de um dos dois anéis permita a passagem. Portanto, girar um anel também pode ajudar a libertar outro. A seleção passa para um anel restante depois da animação.

O objetivo é soltar todos os anéis. Não há tempo limite nem vidas. Pode desfazer ou reiniciar sem penalidade. A geometria calcula as duas interseções entre circunferências; não simula física 3D nem colisão contínua ao longo da trajetória de saída. Os giros são permitidos para explorar alinhamentos; o teste geométrico decide a liberação. Os pinos dourados continuam visíveis e os cantos claros indicam o anel selecionado.

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

| Fase | Anéis | Desafio | Solução construtiva verificada |
|---|---:|---|---:|
| 1 | 2 | Aprender a alinhar abertura, pino e conexão | 8 giros |
| 2 | 3 | Escolher a ordem de uma corrente | 11 giros |
| 3 | 4 | Corrente dobrada, com conexões em direções diferentes | 23 giros |
| 4 | 4 | Circuito fechado: usar uma abertura para ajudar outra | 24 giros |

Os números são comprimentos das sequências produzidas e reproduzidas pelo verificador; não são provas de solução mínima. A dificuldade percebida precisa de avaliação humana. Completar uma fase desbloqueia a próxima; o menu permite trocar entre as fases liberadas, reiniciar, silenciar e sair.

O arquivo `rotate-rings.save` guarda fase, seleção, ângulos, anéis restantes, giros, desbloqueios e som. A gravação usa arquivo temporário e rename. O histórico de desfazer é limitado a 128 ações e existe apenas na sessão atual. Um save inválido é rejeitado e inicia-se uma partida nova. Saves dos outros jogos não são lidos nem modificados.

## Entrega e fontes

- `src/game_logic.hpp`: geometria, fases, regras, verificação de soluções e save.
- `src/renderer.hpp`: texturas próprias geradas uma vez em memória, desenho e animações. Os giros reaproveitam as texturas, evitando redesenhar arcos ponto a ponto a cada frame.
- `src/input.hpp`, `src/audio.hpp`, `src/pixel.hpp`: componentes próprios reaproveitados da coleção, sob [LICENSE-FMG](../../LICENSE-FMG).
- `src/main.cpp`: menu, tutorial, jogo, pausa, vitória e conclusão da campanha.
- `tests/`: regras, save, controles com manches virtuais, vitória/desbloqueio pelo fluxo SDL.

Veja [BUILD.md](BUILD.md), [VALIDACAO.md](VALIDACAO.md) e [REFERENCIAS.md](REFERENCIAS.md). Nenhum código, nível ou asset externo foi copiado. **Compilado para ARM e testado no computador; execução física no M17 pendente.** Não instalado no cartão nesta entrega.
