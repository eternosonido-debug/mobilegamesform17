# Instalação coletiva autorizada — 22/10/2026

17 entradas nativas copiadas para o Ports já funcional da unidade M17/EmuELEC identificada neste projeto. Antes da gravação: backup local conferido de fmg-test e dos launchers existentes; conferência do identificador da partição. Depois: SHA-256 de binários/launchers/assets conferidos, dez arquivos de saves/controles idênticos ao snapshot inicial, remoção de AppleDouble confirmados associados aos jogos e ejeção segura. Nenhum arquivo de boot ou configuração do EmulationStation foi modificado.

| Entrada em Ports | Versão/conteúdo instalado |
|---|---|
| Juices-Sort-Test | motor preservado 1.14, fase/visual existentes |
| New-Blocks-Test | motor preservado 1.14, regras/animações existentes |
| Color-Mix-Test | base inicial 1.14 |
| Sudoku-Test | base inicial 1.14 |
| Traffic-Jam-Test | base inicial 1.14 |
| Mahjong-Test | motor preservado 1.14, visual/animações existentes |
| Boca-Test | motor preservado 1.14, cidade/assets/otimizações existentes |
| Color-Loop-Test | motor preservado 1.14, fases/animações existentes |
| Block-Out-Test | campanha de nove fases |
| Pocket-Beats-Test | 0.6.1, quatro instrumentos e edição de notas em uma tela |
| Screw-Puzzle-Test | 0.1.0 |
| Rotate-Rings-Test | 0.2.0, vinte fases |
| Color-Pop-Test | 0.1.0, vinte fases |
| Tower-Break-Test | 0.2.0, torres de 640/680/720 andares |
| Magic-Tiles-Test | 0.2.0, dez músicas originais / trinta charts |
| Minichef-Test | 0.1.0, três receitas |
| Cooking-Test | 0.1.0, confeitaria livre / quatro doces por conjunto |

Cada executável fica em `/sdcard/fmg-test/bin/<slug>-test`; launchers em `.config/emuelec/ports` na partição de jogos; saves em `/sdcard/fmg-test/saves`. Mantido o `controls.sh` já presente no cartão. Os cinco launchers antigos foram preservados, Pocket Beats atualizado e os demais acrescentados. Assets de Boca mantidos/conferidos e cenário de Cooking acrescentado em `fmg-test/assets/cooking/kitchen.bmp`.

Os três jogos básicos (Color Mix, Sudoku e Traffic Jam) são as bases existentes, sem refinamento próprio nesta entrega. Cópia validada e inspeção estática dos 17 executáveis **não equivalem a execução em hardware**. Testar no M17 físico legibilidade, áudio, ambos os manches, A/B/X/Y, Start/Select, retomada, desempenho e retorno ao Ports. Bibliotecas proprietárias do firmware, backups e saves não são publicados.
