Assunto: Entrega do trabalho de ordenação por blocos

Prezado(a) Professor(a),

Segue em anexo a entrega do trabalho de ordenação por blocos.

A implementação foi organizada com uma estrutura um pouco mais modular para facilitar a comparação entre as três versões solicitadas: sequencial, `std::thread` e `OpenMP`. A ideia foi evitar duplicação de código e garantir que todas as versões executassem exatamente a mesma lógica principal, mudando apenas a forma de paralelizar a ordenação dos blocos.

Alguns pontos importantes da implementação:

- os vetores são divididos dinamicamente em blocos de tamanho 4 ou 5;
- blocos pares e ímpares são ordenados com algoritmos diferentes, conforme definido pela combinação escolhida;
- a etapa final consolida os blocos para formar o vetor ordenado;
- as três versões usam os mesmos vetores de entrada e as mesmas sementes de particionamento, para manter a comparação justa;
- foram adicionadas validações para verificar se os blocos têm tamanho correto, se o resultado final está ordenado e se os elementos originais foram preservados;
- os resultados são exportados em arquivos para permitir a geração dos gráficos usados no relatório.

Também foi implementada uma etapa automática de escolha da melhor combinação de algoritmos. Em vez de definir manualmente uma combinação fixa, o programa avalia as combinações permitidas pelo enunciado, respeitando a regra de não repetir o mesmo algoritmo em etapas diferentes. A combinação com menor tempo médio é então usada nas execuções comparativas.

O desenvolvimento foi realizado em macOS. Por isso, o `Makefile` inclui uma configuração específica para uso da biblioteca `libomp`, necessária para compilar e executar a versão com `OpenMP` nesse ambiente. Em sistemas Linux, normalmente a flag `-fopenmp` já é suficiente, mas no macOS foi preciso indicar os caminhos de inclusão e linkedição da `libomp`.

Essa estrutura acabou deixando o código um pouco mais elaborado do que uma implementação direta, mas a intenção foi tornar os testes mais reprodutíveis e a comparação entre as versões mais confiável.

Atenciosamente,

João
