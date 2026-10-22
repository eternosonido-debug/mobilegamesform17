# Pesquisa prévia — Screw Puzzle

Data: 22/10/2026. Pesquisa feita antes de criar o código nativo do jogo. Nenhum código externo foi incorporado; nenhuma instalação no SD foi realizada.

## Candidatos conferidos

| Projeto | Evidência | Aproveitamento avaliado |
|---|---|---|
| [ScrewJam — sidhant947](https://github.com/sidhant947/ScrewJam) | Fonte baixada na revisão `5ee0b84970eba12d4fd7f937bae162bf75ba61f3`. LICENSE GPLv3; pubspec.yaml confirma Dart/Flutter, Flame, Riverpod e Hive. Foram inspecionados modelos de placas/parafusos, partes do renderer e gerador. | É o candidato de mecânica mais próximo: placas sobrepostas, suporte por parafusos e seleção por camadas. Não há um executável SDL/M17 pronto nessa base. Portar seu código exigiria adaptação de Dart/Flutter para nosso ambiente e preservação da licença/créditos das partes derivadas. Não importar toda a infraestrutura mobile. |
| [GameNutsAndBolts — dangxlam](https://github.com/dangxlam/GameNutsAndBolts) | Repositório C++ com dois arquivos e README. Não foi encontrada licença de reutilização na raiz nem uma distribuição completa para M17. | Não adotar como código reutilizável sem uma autorização de licença verificável. Ser C++ não basta para confirmar compatibilidade nem direito de cópia. |
| [Screw Out — theprantadutta](https://github.com/theprantadutta/screw_out_flutter_app) | README informa Flutter e reserva expressamente os direitos do jogo, arte, níveis e sons; permite leitura pública, não redistribuição. | Excluir como fonte de código/assets para cópia. |

Também foram pesquisados repositórios para “screw puzzle”, “nuts bolts game” e “unscrew game”. A maioria dos resultados próximos usa Unity/Flutter ou não informa uma licença identificável. Esta busca não comprova que inexista outra alternativa adequada.

## O que já pode poupar trabalho na coleção

A base existente possui controle de ambos os manches/D-pad e A/B/X/Y, primitivas/fontes de pixel art, salvamento local, menus, wrappers de Ports, testes SDL sem tela e a toolchain ARM soft-float recuperada da unidade. Esses componentes próprios têm licença MIT (LICENSE-FMG) e evitam começar a infraestrutura do próximo jogo do zero.

## Decisão técnica para a próxima etapa

Não tratar nenhum dos projetos encontrados como jogo nativo M17 pronto. Reutilizar primeiro a infraestrutura própria SDL2/ARM e avaliar isoladamente qualquer algoritmo externo antes de copiar ou traduzir. ScrewJam permanece como candidato para estudar organização de camadas e suporte; sua implementação não foi adaptada nesta etapa. Caso código derivado dele seja incorporado, registrar exatamente a origem, alterações e termos aplicáveis no módulo.

Estado ao concluir esta pesquisa: especificação do Screw Puzzle existente; implementação nativa ainda não iniciada. Não foi executado nenhum candidato no M17.

## Resultado da implementação — 22/10/2026

Implementada a versão 0.1.0 original C++11/SDL2, com três fases próprias e busca de soluções. Nenhum código, nível ou asset dos candidatos externos foi copiado ou traduzido. Foram reutilizados os arquivos próprios de entrada, fonte/primitivas e efeitos sonoros do Block Out, sob LICENSE-FMG, mais o helper genérico `shared/fmg-runtime/src/direction_input.hpp`. A pesquisa poupou a tentativa de portar um stack Flutter inadequado à base atual. Testes locais, build ARM e inspeção de símbolos documentados em VALIDACAO.md; hardware pendente.
