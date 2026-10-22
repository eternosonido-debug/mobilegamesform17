# Pesquisa anterior à implementação — Cooking, 22/10/2026

## Referências fornecidas

33 capturas em `Photos-1-001.zip`: escolha de sabor, adicionar ingredientes à tigela, mexer, preencher quatro formas, colocar no forno e decorar cupcakes com cobertura, confeitos e apresentação. São referência de dinâmica, não arquivos de arte para distribuição. Uma captura do sistema do telefone não participa do jogo. Não incorporar textos/interface/personagens/assets dessas capturas. Estética solicitada: plataforma 16-bit colorida, identidade própria.

## Pesquisa no GitHub

- [Yiruzz/CookedBattle](https://github.com/Yiruzz/CookedBattle), revisão `5506fd66afd6d2315a0c859365101bd1dfdf303e`. Lidos inventário, `LICENSE` e `project.godot`: MIT, copyright 2023 Yiruzz, Godot 4.0/Forward Plus, cenas/GDScript, projeto de luta+cozinha. Incompatível diretamente com o runtime SDL2 ARM32 e com a confeitaria livre solicitada; não copiar código, áudio ou assets. Licenças de todos os assets não foram auditadas, pois não serão usados.
- [nothings/stb](https://github.com/nothings/stb), revisão `a7d4c2175630e1ea5b6f8dd00ab96fed0b97b96d`: inspecionados inventário e `LICENSE`, alternativas MIT (Sean Barrett, 2017) ou domínio público. Considerado `stb_image` para carregar PNG sem SDL_image. Não incorporado: uma cópia BMP otimizada pode ser lida pelo próprio SDL_LoadBMP, evitando dependência adicional e preservando a pipeline conhecida do aparelho.
- Componentes próprios inspecionados de Minichef: fonte, entrada configurável para ambos os manches, efeitos sonoros, cross-build, teste SDL e launcher Ports. Reutilizar esses componentes sob LICENSE-FMG, preservando Minichef sem alterações.

## Decisão antes de codificar

Criar jogo novo independente em `games/cooking`: regras de confeitaria passo a passo (não corrida de atendimento), três massas, animações de verter/quebrar ovos/misturar/porcionar/assar, decoração livre e álbum local de doces. Cenário novo gerado pela ferramenta integrada de imagens, mantido no projeto; demais objetos e animações originais desenhados por código SDL. BMP em 480×272, cache em textura; sem recursos comerciais ou bibliotecas do firmware distribuídas. Regras puras, save validado, controles físicos, teste local e cross-build; desempenho no M17 continuará pendente até executar no aparelho.
