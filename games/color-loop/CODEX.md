# Manutenção — Color Loop

Leia o AGENTS.md da raiz, este README e docs/CONTROLS.md. Esta pasta recupera código real já existente. Preserve o comportamento e os saves; não converta automaticamente os gráficos para a proposta de pixel art.

A camada compartilhada preserva o motor legado para evitar regressões. Mudanças em shared/fmg-runtime afetam os oito títulos e exigem executar seus testes. Módulos específicos podem ser modificados nesta pasta; não mexer nas pastas dos jogos planejados.

Antes de declarar compatibilidade no aparelho, validar na unidade física. O input de ambos os manches foi testado com dispositivos SDL virtuais; o direito físico pode depender dos eventos que o firmware expõe.
