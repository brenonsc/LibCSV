FROM ubuntu:latest

RUN apt-get update && apt-get install -y gcc

WORKDIR /app
COPY . /app

RUN chmod +x build.sh && ./build.sh

CMD ["./app"]