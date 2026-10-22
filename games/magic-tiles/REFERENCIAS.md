# Pesquisa prévia — Magic Tiles, 22/10/2026

Antes da implementação: busca GitHub por rhythm game SDL2/four lanes/MIT, inspeção de licenças e infraestrutura própria.

| Projeto/revisão | Evidência e decisão |
|---|---|
| [rhythm-game-utilities/rhythm-game-utilities](https://github.com/rhythm-game-utilities/rhythm-game-utilities), `d52af47134d7f8f15e5dd2448d35290ccbc2f50d` | MIT, Scott Doxey 2026. README, LICENSE e headers Utilities/Common/Accuracy inspecionados. Common utiliza std::clamp (C++17) e regex; a biblioteca abrange importação MIDI/.chart que não precisamos neste MVP C++11. Não incorporar a infraestrutura inteira para poucas janelas de tempo. |
| [ctbot000/rhythm-game](https://github.com/ctbot000/rhythm-game), `1a42d2e082cdf3890b4fca886484973db49df0e2` | MIT, navegador/Web Audio. README e licença conferidos: quatro pistas, áudio sintetizado, relógio de áudio e calibração. Boa referência arquitetural; não importar navegador, músicas, charts ou identidade. |
| [Bambaclad1/4key-rhythmgame](https://github.com/Bambaclad1/4key-rhythmgame), `1f1fa43750c9c3e6d5a6bd873721ba248dda609f` | C++/SFML, projeto Windows/Visual Studio. license.md contém a licença do SFML, sem autorização própria clara para o restante do jogo e recursos. Não copiar código/assets sem esclarecer os direitos. |

## Decisão antes do código

Reutilizar fonte, primitivas, controles configuráveis, build/Ports e save próprios da coleção (LICENSE-FMG). Reutilizar a arquitetura de áudio própria do Pocket Beats: amostras pré-calculadas, mixagem em callback SDL e relógio publicado por operações atômicas, independente da interface. Criar três composições, charts, timbres e julgamentos originais. Não incorporar código, charts, áudio ou arte externos nesta versão. Sem candidato executado no M17; pesquisa não prova inexistência de outras alternativas.
