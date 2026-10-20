# Juices Sort 1.1 — visual e animações

Novo painel claro, tubos com reflexos e sombras, seleção destacada, cores numeradas, animação de levantar/inclinar/despejar e gotas. Avisos indicam tubos vazios, cheios e cores incompatíveis. Ajuda e celebração de vitória redesenhadas.

A seleciona e despeja; B cancela; X abre ajuda; Y desfaz; START abre menu; SELECT salva e sai. Durante o despejo, os comandos de movimento aguardam o fim da animação; START continua disponível. O movimento é salvo inteiro, sem estados de líquido pela metade. Salvamentos anteriores continuam no formato FMG 1.

Validação: build local com AddressSanitizer/UndefinedBehaviorSanitizer, testes de regras/salvamento e animação, seis telas de prévia. Binário ARM soft-float compilado com a SDL do cartão e inspecionado estaticamente. Esta atualização ainda precisa de teste visual no aparelho.

A instalação neste cartão substitui somente fmg-test/bin/juices-sort-test. O atalho Ports existente, o menu original e os salvamentos são preservados.

Prévia reproduzível: --smoke --juice-preview board|selection|pour|success|help|invalid --screenshot tela.bmp.
