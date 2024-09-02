# MO601 - Arquitetura de Computadores II 2024/2 Projeto 1
Este projeto faz parte das atividades avaliativas da disciplina e consiste em desenvolver um simulador de processador RISC-V com a arquitetura RV32IM. O simulador deve ser capaz de receber um arquivo binário e executá-lo de maneira correta. O professor forneceu alguns benchmarks para que o simulador pudesse ser testado. Este texto é um relatório detalhado do desenvolvimento desse projeto.
# Instruções para a execução do programa
0. Antes de executar o programa é necessário conferir os compiladores disponíveis na máquina. É preciso ter o Python3, o gcc e a riscv-gnu-toolchain também.
1. Primeiro execute o comando
```
make all
```
para compilar o simulador.
Em seguida, escolha o script adequado para compilar a pasta `test`. Se o comando para compilar em riscv na sua máquina for riscv64-unknown-elf, use o comando
```
python3 test/scripts/compile64_tests.py
```
Caso seu compilador seja o riscv32-unknown-elf, use o comando
```
python3 test/scripts/compile32_tests.py
```
Se seu compilador for outro, por favor altere o script de compilação na linha 30:
```C
compile_command = ['riscv32-unknown-elf-gcc', '-g', '-march=rv32im', '-std=gnu99', '-mabi=ilp32',
                           aux_file, source_file, '-o', output_file,
                           '-g', '-march=rv32im', '-std=gnu99', '-mabi=ilp32', '-nostartfiles', '-nostdinc', '-nostdlib']
```
2. Para executar as simulações, chame o script:
```
python3 test/scripts/execute_tests.py
```
3. Os logs de saída ficarão na pasta `test_log`

# Descrição do ambiente de desenvolvimento
Para a realização deste trabalho foi utilizado um computador com sistema operacional Linux (Ubuntu) com os compiladores gcc (instalado por padrão) e o [riscv-gnu-toolchain](https://github.com/riscv-collab/riscv-gnu-toolchain) (instalado pelo usuário). Outra ferramenta instalada foi o simulador [spike](https://github.com/riscv-software-src/riscv-isa-sim) juntamente com o [riscv-pk](https://github.com/riscv-software-src/riscv-pk). 
Além disso, para depuração, foi utilizado o debugger do Visual Studio Code presente na [extensão C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools). Também foi utilizado o Python para a escrita de scripts de compilação e de execução.

## Referências utilizadas para o projeto
Uma vez levantado o ambiente, é necessário obter informações sobre a ISA do RISC-V RV32IM para que a simulação siga o padrão estabelecido.
A principal referência é o [Instruction Set Manual do RISC-V](https://riscv.org/technical/specifications/) , que tem a maioria das informações necessárias.  Ela possui:

- A lista de todas as instruções das diversas extensões;
- O formato de cada instrução com seus valores de funct3,funct7, etc;
- A nomenclatura da ABI para os registradores.

Foi também utilizado o [Guia Prático RISC-V](http://riscvbook.com/portuguese/) que apresenta as instruções num formato mais prático de leitura, pois a descrição de cada instrução é bem completa. Além do nome e do formato da instrução, o guia também descreve o comportamento da mesma.
A principal referência para o entendimento do formato dos arquivos ELF foi a sua [página da wikipédia](https://pt.wikipedia.org/wiki/Executable_and_Linkable_Format), que é bem completa.

## Processo de Compilação
Para fazer a compilação dos arquivos fonte em C foi utilizada a toolchain do gnu com diversas flags. Veja um exemplo de comando para compilar um arquivo chamado 000.main.c:
`riscv64-unknown-elf-gcc -g -march=rv32im -std=gnu99 -mabi=ilp32 crt.S \
000.main.c -o 000.main.elf -g -march=rv32im -std=gnu99 -mabi=ilp32 \
-nostartfiles -nostdinc -nostdlib`
Essas flags foram copiadas diretamente do repositório do [ACStone](https://github.com/rjazevedo/ACStone) e elas servem, entre outras coisas, para gerar apenas instruções dentro da arquitetura RV32IM e para impedir a geração de instruções comprimidas.

## Scripts para automatização
Como a lista de arquivos a serem compilados e executados é grande, foram escritos scripts na linguagem Python para que tudo ocorresse de forma automática. Inicialmente foi gerado um script base com o ChatGPT e em seguida ele foi modificado para atender às necessidades do projeto.
O primeiro script, `compile_tests.py` compila (com o compilador de RISC-V) todos os arquivos `.c` presentes na pasta indicada e guarda os binários em outra pasta que também pode ser indicada.
O segundo script, `execute_tests.py`, lê todos os arquivos `.elf` de uma pasta e executa o simulador para cada um deles. Ele guarda os arquivos de `.log` em outra pasta.

# Descrição do algoritmo de simulação
Podemos dividir o algoritmo de simulação em três grandes etapas:

- Leitura e carregamento do arquivo `.elf`;
- Execução das instruções;
- Log das instruções.

As próximas seções apresentaram o princípio de onamento de cada etapa, além de algumas ideias utilizadas e desafios encontrados.

## Leitura e carregamento do arquivo `.elf`
O algoritmo do programa abre o arquivo `.elf` de entrada no formato binário e passa pelos cabeçalhos do arquivo, byte a byte. Ele identifica onde estão os trechos relevantes para a simulação e carrega eles para a memória.
Inicialmente tentou-se ler o `.elf` no modo de programa, usando os *Program Headers*. Com eles é possível identificar os segmentos carregados (LOAD) e colocá-los na estrutura de dados que simula a memória. Não foi possível usar essa abordagem porque mapear a memória do programa para a estrutura de dados criada para simular a memória mostrou-se muito difícil.
O método utilizado foi ler o arquivo `.elf` no modo *linker*, usando os *Section Headers*. Todas as seções com seus respectivos nomes puderam ser identificadas. Assim, era possível separar a memória de dados e a memória de programa, abordagem mais compatível com a estrutura de dados.
A estrutura de dados da memória foi codificada da seguinte maneira:
```C
typedef  struct SimulatedMemory{
TextSegment text_seg;
DataSegment rodata_seg;
DataSegment data_seg;
DataSegment sdata_seg;
unsigned  char  misc_seg[MAX_DATA];
} SimulatedMemory;
```
Ela consiste em um bloco do tipo `TextSegment` para guardar as informações das instruções e quatro blocos do tipo `DataSegment` para guardar informações de variáveis inicializadas. Além disso, há um vetor de bytes chamado `misc_seg` para representar o restante da memória. Certamente esse campo tem tamanho limitado e a tática para lidar com isso é explicada na seção de acesso à memória.
O importante de se entender sobre os segmentos da memória é que cada segmento é tratado como uma parte independente, tendo seu endereço inicial e tamanho. O segmento `text_seg` ainda apresenta uma estrutura especial para tratar as instruções. Esse tratamento será explicado na seção de execução das instruções.

## Execução das instruções
A execução as instruções foi separada em duas etapas: *fetch* e *execute*.

### Fetch
Na primeira etapa a instrução é identificada a partir do PC e a estrutura de dados correspondente é buscada da memória simulada. Todas as instruções são armazenadas no seguinte formato:
```C
typedef  struct  InstData {
int  raw_instruction;
int  opcode,
rd,
rs1,
rs2,
imm,
funct3,
funct7;
InstType  type;
} InstData;
```
`raw_instruction` nada mais é do que os 32 bits da instrução armazenados diretamente.  O campo `type` identifica o tipo da instrução como I,J,R, etc. Os demais campos são auto-explicativos.
Os campos são preenchidos a partir da decodificação de `raw_instruction`. Existe uma função que faz isso.
```C
InstData  decode_instruction(int  raw_instruction)
```

Ela segue os seguintes passos:
1. O *opcode* é lido e a partir dele é identificado o tipo da instrução;
> Nota: foi observado que o RV32IM possui uma quantidade pequena de *opcodes* (veja [esta discussão](https://www.reddit.com/r/RISCV/comments/1boswda/why_dont_riscv_opcodes_more_directly_correspond/) no reddit),  de forma que era possível mapear rapidamente cada instrução para o seu tipo.
2.  De posse do tipo da instrução, os campos podem ser preenchidos adequadamente.

### Execute
A execução das instruções é feita uma a uma por uma função:
```C
int  execute_inst(int  inst_index, SimulatedMemory  *simulated_memory, int  reg_bank[], int  *PC, int  *cycles)
```
Sobre os argumentos da função: o banco de registradores `reg_bank[]` é apenas um vetor do tipo `int` de 32 posições. `inst_index` serve para que a função encontre a instrução dentro de `simulated memory` (o acesso à memória de instruções é diferente do acesso às demais regiões).  PC tem o endereço da instrução e `cycles` é uma varável para contar os ciclos de execução.
A função primeiro encontra a instrução dentro da memória e em seguida  a identifica. Para descobrir qual instrução está sendo executada, é feita uma cadeia de `if-else` testando as instruções uma a uma. Nessa situação, não basta ver o `opcode` da instrução. Em muitos casos é preciso ver os campos funct3 e funct7 (instruções tipo R) ou até mesmo alguns bits do imediato (instruções slli e srli). Nas instruções do tipo ecall, é necessário ler os registradores de argumento para decidir qual operação deve ser realizada. A lista com os números das syscalls foi encontrada [nessa página](https://jborza.com/post/2021-05-11-riscv-linux-syscalls/).
Uma vez que a instrução é devidamente identificada, ela é executada. É necessário tomar bastante cuidado nessa parte pois existem muitas instruções que fazem quase a mesma operação. slt e sltiu, por exemplo, diferem apenas no fato de que a primeira trata os dados como com sinal e a segunda como sem sinal.
A instrução ebreak foi considerada como finalizadora de programa. As instruções FENCE foram ignoradas porque o simulador feito é *single core*.
Ao final da execução de cada instrução é entregue um código de retorno para a simulação. Ele serve para avisar que o programa acabou ou então que ocorreu algum erro.

### Acesso à memória

O acesso à memória é feito com duas funções:
```C
void  memory_write(int  num_bytes, int  addr, SimulatedMemory  *simulated_memory, int  value)
void  memory_read(int  num_bytes, int  addr, SimulatedMemory  *simulated_memory, int  *value)
```
Essas funções são capazes de ler e escrever números diferentes de bytes na memória, indicado bom `num_bytes`. Com essa configuração é possível fazer com que só essas duas funções sirvam para todas as instruções sw/lw, sh/lh e sb/lb.
O passo a passo das funções é o seguinte:
1. Primeiro é identificado o segmento de memória que a instrução deseja acessar. Pode ser o `rodata_seg`, `data_seg`, `sdata_seg` ou `misc_seg`. Não são permitidos acessos ao `text_seg`;
2. Se o endereço não mapear para nenhum dos quatro campos, `misc_seg` é usado como um campo coringa que mapeia o endereço requisitado usando `addr%MAX_DATA`, onde `MAX_DATA` é o tamanho de `misc_seg` (64KB);
3. Uma vez que foi decidido qual segmento da memória será acessado, os dados são carregados ou lidos byte a byte de acordo com `num_bytes`.

A estratégia de mapear a memória para uma região coringa ona bem para programas pequenos, mas pode acontecer de dois acessos à memória caírem no mesmo byte de `misc_seg` no caso de programas maiores.

## Log das instruções
Como sugerido pelo professor, foi feita uma estrutura de dados para armazenar as informações relevantes para o log de execução.
```C
typedef  struct  ExecLogEntry{
InstData  inst_data;
int  PC,
rd_index,
rs1_index,
rs2_index,
rd_value,
rs1_value,
rs2_value;
} ExecLogEntry;
```
`ExecLogEntry` corresponde a uma linha do log. A estrutura de dados `inst_data` já foi explicada anteriormente e é utilizada para imprimir o mnemônico da instrução. Os demais campos são usados para imprimir as informações de execução.
O log é imprimido linha a linha por meio de uma função e é tudo armazenado num arquivo `.log`.

# Descrição de como o projeto foi testado
Do ponto de vista de testagem, o desenvolvimento do projeto pode ser separado em duas etapas
- Decodificação do arquivo `.elf`;
- Simulação das instruções.

## Decodificação do arquivo `.elf`
Antes de começar a testar a execução das instruções é necessário verificar se o programa está lendo o arquivo executável corretamente. Para isso, deve-se conferir se as instruções lidas e se os dados de inicialização de variáveis estão corretos.
No início do desenvolvimento do projeto, era gerada uma saída que apresentava apenas as instruções com seus mnemônicos e os dados de inicialização. Essa saída era então comparada com o resultado do *dump* do executável como em:
`llvm-objdump -D 000.main.elf`
O arquivo de saída do projeto e o arquivo de saída do `llvm-objdump` eram então modificados para terem as linhas no mesmo formato. Esses arquivos de texto eram então comparados utilizando o comparador nativo do VS Code. Foram feitos testes para cerca de uma dezena de arquivos do ACStone.

## Simulação das instruções
Uma vez que o arquivo `.elf` estava sendo lido corretamente, deu-se início à escrita do código responsável por executar as instruções. Para conferir se o simulador estava onando corretamente, foi utilizado o [simulador spike](https://github.com/riscv-software-src/riscv-isa-sim) juntamente com o [riscv-pk](https://github.com/riscv-software-src/riscv-pk). Também foram feitas análises manuais em alguns casos.

### Spike
O spike é um simulador de RISC-V *open source* disponível no GitHub. O simulador sozinho é capaz de executar muitas simulações, mas ele precisa do riscv-pk para tornar o processo mais fácil. Enquanto o spike executa no *bare-metal*, o pk cria um ambiente de execução mais controlado para a simulação.
A instalação do spike é simples, mas a do riscv-pk se mostrou bastante complicada. Foi necessário recompilar a riscv-gnu-toolchain algumas vezes até que a versão correta da ferramenta fosse instalada. Para que o pk de 32 bits funcionasse, foi preciso instalar a riscv-gnu-toolchain com as variantes, de acordo com [essa issue](https://github.com/FreeRTOS/FreeRTOS/issues/830): 
```
./configure --with-arch=rv32i_zicsr_zifencei --with-abi=ilp32
```
Usar a flag `--multi-lib` foi tentado e não funcionou.
Com o riscv-pk instalado foi possível tentar alguns testes, mas sem sucesso. O log de execução do spike mostrava diversas instruções do kernel (400mil instruções) mas o programa quebrava assim que iniciava a execução do código do ACStone. O spike acusava acessos indevidos à memória.
Depois das tentativas mal sucedidas, percebeu-se que executar o spike em *bare-metal* funcionava, desde que a configuração de memória fosse feita corretamente com a flag `-m`.  Também era necessário indicar a arquitetura de 32 bits com `--isa=rv32im`. O único problema dessa abordagem é que o processador ficava preso em um loop infinito por não conseguir tratar o fim do programa. A solução para isso foi forçar o encerramento da simulação com o comando do `timeout`  do *shell*. O comando final para simular com o spike foi o seguinte:
```
$ timeout 0.01s spike -m10000:1000000 -l --isa=rv32im 000.main.elf &>spike.log
```
Depois desse tempo descobrindo como simular com o spike, foi possível finalmente obter os logs de execução desejados. Os logs do spike eram então comparados com os logs do simulador desenvolvido para verificar se tinha algo de errado.
Com a simulação do spike foi possível, por exemplo, encontrar um bug nas funções de bit shift slli, srli e srai. Note que nessas funções existe um campo identificador nos 6 bits mais significativos da instrução. Veja um exemplo na imagem abaixo, retirada do [guia](http://riscvbook.com/portuguese/) :
![instrução bit shift](/images/srai.png)
O bug encontrado no simulador surgiu porque esse campo identificador foi confundido com o funct7, que fica na mesma posição. Em vez de tratar os 6 bits, eram considerados 7 e assim a instrução executada era trocada.

### Análises manuais
O log da simulação do spike, da maneira mostrada anteriormente, mostra apenas o fluxo de execução das instruções. Não foi possível encontrar uma opção de log que também mostrasse os valores da memória ou dos registradores. Por isso, foram feitas algumas análises manuais dos códigos do ACStone. Foi escolhido um código de cada tipo (i.e. const, cast, add, sub, mul,etc).
Essas análises consistiam em acompanhar o código em C e tentar compará-lo com a execução dele no simulador. Para isso era necessário, por exemplo, identificar quais posições da memória estavam sendo reservadas para cada variável e imprimir na tela essas posições. 
Com essa abordagem foi possível encontrar bugs como, por exemplo, o bug do zero. Esse bug se manifestou da seguinte forma: como o banco de registradores é um vetor que varia dinamicamente, o registrador zero pode ser alterado livremente. Instruções como
`jal zero, 0x1000`
escreviam no registrador zero e deixavam ele com um valor inválido. A solução para isso foi refrescar o valor de zero ao fim da execução de cada instrução.

# Considerações sobre o aprendizado
Este trabalho foi muito proveitoso para ampliar os conhecimentos de arquitetura de computadores assim como os de execução de arquivos binários e simulação.
Implementar um simulador para uma arquitetura traz um conhecimento mais detalhado sobre ela, fazendo com que se percebam coisas que antes não eram notadas. Apareceram pseudo instruções nunca antes vistas por mim durante as simulações, como a `not rd, rs1`. As instruções slli, srli e srai são uma exceção na arquitetura e têm uma parte do imediato que funciona quase como um funct7. Os opcodes do RV32IM são poucos e podem ser rapidamente mapeados para o tipo da instrução.
Foi preciso fazer um estudo detalhado dos arquivos `.elf` para poder decidir qual a melhor abordagem para carregá-lo no programa. Apesar de a forma utilizada nesse projeto (usando os *Section Headers*) funcionar, talvez seja interessante mudá-la e buscar carregar o programa diretamente pelos *Program Headers*, tornando o procedimento mais parecido com uma execução real.
Foi aprendido também que a simulação de um processador depende de muitos detalhes. Se não foi carregado um *kernel* antes da execução, por exemplo, o processador não consegue lidar com exceções. Além disso, foi aprendida a importância de se verificar se acessos à memória são válidos.

# Desempenho do simulador conforme o Dhrystone e CoreMark
Foi tentado realizar a simulação do *Dhrystone*, mas foram encontrados diversos erros de compilação que não puderam ser resolvidos dentro do tempo disponível. O código depende de muitas macros que eu nunca havia visto antes e não consegui entender muito bem qual era o problema da compilação. Por exemplo, ao usar o makefile do próprio repositório, aparecia um erro de que a variável `__riscv_xlen` tinha um valor inválido.
Com relação ao CoreMark, foi possível compilá-lo e executá-lo facilmente na minha máquina x86, mas não consegui preparar o benchmark para meu simulador de RISC-V no tempo disponível. Apesar de existir um guia para portar a compilação para outras arquiteturas, o passo-a-passo era muito longo e não foi possível terminá-lo a tempo.
