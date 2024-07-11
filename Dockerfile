# Escolha uma imagem base
FROM ubuntu:latest

# Instale o compilador GCC e outras ferramentas necessárias
RUN apt-get update && apt-get install -y gcc

# Defina o diretório de trabalho
WORKDIR /app

# Copie os arquivos do seu projeto para o diretório de trabalho
COPY . /app

# Dê permissão de execução ao script de build e execute-o
RUN chmod +x build.sh && ./build.sh

# Comando para executar sua aplicação
CMD ["./app"]