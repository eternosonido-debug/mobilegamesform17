# Validação — Cooking 0.1.0, 22/10/2026

- Compilação nativa C++11/SDL2 com Apple Clang, `-Wall -Wextra -pedantic`, AddressSanitizer e UndefinedBehaviorSanitizer. Quatro grupos específicos passaram: regras/save, smoke SDL, integração com controles e integração de decoração/foto/álbum.
- Regras: seis etapas em cada um dos três sabores; seleção, bloqueio de ingrediente repetido, gesto de mistura inválido, transferência às formas, forno e proibição de retirar massa crua; aplicação/desfazer e preservação de quatro desenhos. Álbum com dez criações para verificar a rotação de oito espaços. Saves em todas as etapas, inclusive no meio da animação de ingrediente; rejeição de índices e arquivos inválidos sem substituir o estado carregado.
- Integração: dois manches virtuais (eixos 0 e 2), A com derramamento completo, Y/B ajuda, Start/B pausa/retomada, Select com confirmação, save e saída. Decoração retomada de save, aplicação, desfazer, troca de doce, conclusão e álbum persistido.
- Capturas dos seis estados do renderer SDL feitas no computador; cenário BMP carregado, cache de quatro doces e variantes de decoração exibidos. Não são imagens do M17.
- Regressão geral da coleção: **50/50 grupos passaram em 74,49 s**. Após os últimos ajustes apenas de apresentação/síntese de efeitos, os quatro grupos específicos foram repetidos, passando em 8,87 s. Sem avisos de compilação.
- ARM: ELF ARM32 soft-float Cortex-A9 gerado com Zig e a biblioteca SDL 2.28.5 recuperada do aparelho. Inspeção de 540 importações fortes/dependências/versionamentos sem falhas; `arm-compatibility.json`. Os demais 16 executáveis também tiveram suas importações e ABI verificadas antes da instalação coletiva.

## Instalação autorizada pelo usuário

Cooking foi acrescentado pelo launcher próprio Ports, junto dos demais jogos disponíveis, no cartão previamente identificado. Binário, launcher e cenário copiados com conferência SHA-256; backup anterior, saves e controles preservados. Os arquivos de boot/configuração de sistemas não foram alterados. Cópia no SD **não equivale a execução no console**.

## Pendente no M17 físico

FPS/carga real, latência e som, legibilidade das legendas, conforto da mistura com os dois manches, todos os eventos físicos dos botões, estabilidade das texturas e retorno ao Ports. Não foi feito teste em hardware nesta versão. Controles digitais virtuais não confirmam os identificadores do firmware. Não há touchscreen exigido, rede, gerenciamento de restaurante ou música contínua.
