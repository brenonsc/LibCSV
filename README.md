# LibCSV 📚
![Language](https://badgen.net/badge/Language/C/green?icon=)
![License](https://badgen.net/badge/License/MIT/purple?icon=)
![Docker](https://badgen.net/badge/icon/Available?icon=docker&label)

LibCSV é uma biblioteca em C para processamento de arquivos CSV, permitindo a aplicação de filtros e a seleção de colunas conforme especificado. A biblioteca é projetada para ser utilizada como uma biblioteca compartilhada (.so) em aplicações C.
<br>

## Objetivo ✅
Implementar uma que faça a seleção de colunas e aplicação de filtros de forma eficiente. A biblioteca oferece funções para processamento de dados CSV diretamente de strings ou arquivos, produzindo saídas filtradas e formatadas conforme especificações fornecidas pelo usuário. Ideal para aplicações que necessitam de manipulação dinâmica e precisa de grandes volumes de dados CSV, LibCSV suporta filtros de comparação (>, <, =, !=, >= e <=) e garante a integridade e a ordem das colunas e filtros, atendendo a requisitos de desempenho e compatibilidade com a arquitetura x86_64.
<br>

## Testando a aplicação 👨‍🔬
Para configurar a aplicação para executar em ambiente local, é necessário ter instalado o Docker, e assim seguir o passo a passo abaixo:

#### 1. Clone o Projeto

```bash
git clone https://github.com/brenonsc/LibCSV.git
cd LibCSV
```

#### 2. Construa a imagem no Docker

```bash
docker build -t libcsv .
```
**Observação:** Caso o sistema em uso seja de uma arquitetura diferente de x86_64, é necessário adicionar a flag `--platform=linux/arm64`.

#### 3. Rode o contêiner

```bash
docker run -d libcsv
```
<br>

## Funções da biblioteca e seus respectivos atributos  :pushpin:
processCsv
- csv: String com os dados do CSV, onde cada linha representa um registro e as colunas são separadas por vírgulas.
    - Exemplo: `"header1,header2,header3\n1,2,3\n4,5,6"`
- selectedColumns: Uma string onde os nomes das colunas a serem selecionadas são separados por vírgulas.
    - Exemplo: `"header1,header3"`
- rowFilterDefinitions: Uma string onde cada filtro é definido em uma nova linha, no formato `header(comparador)valor`.
    - Exemplo: `"header1>1\nheader2=2\nheader3<6"`

processCsvFile
- csvFilePath: String com o caminho do arquivo CSV.
    - Exemplo: `"path/to/csv_file.csv"`
- selectedColumns: Uma string onde os nomes das colunas a serem selecionadas são separados por vírgulas.
    - Exemplo: `"header1,header3"`
- rowFilterDefinitions: Uma string onde cada filtro é definido em uma nova linha, no formato `header(comparador)valor`.
    - Exemplo: `"header1>1\nheader2=2\nheader3<6"`

<br>

Exemplo:

```c
const char csv[] = "header1,header2,header3\n1,2,3\n4,5,6\n7,8,9";
processCsv(csv, "header1,header3", "header1>1\nheader3<8");

// output
// header1,header3
// 4,6

const char csv_file[] = "path/to/csv_file.csv";
processCsvFile(csv_file, "header1,header3", "header1>1\nheader3<8");

// output
// header1,header3
// 4,6
```

<br>

## Testes Unitários :test_tube:
A aplicação contempla testes unitários utilizando-se de CUnit, e está presente dentro da pasta `test`. Caso queira, os mesmos podem ser acessados da seguinte forma (após a clonagem do repositório):

#### 1. Acesse o diretório

```bash
cd test
```

#### 2. Rode o comando make

```bash
make
```

#### 3. Rode os testes

```bash
./test_libcsv
```
<br>

## Licença 

Este software está licenciado sob a [Licença MIT](https://github.com/brenonsc/LibCSV/blob/main/LICENSE).
