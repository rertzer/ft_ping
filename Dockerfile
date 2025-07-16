FROM debian:bullseye-slim
RUN apt update
RUN apt upgrade
RUN apt install inetutils-ping=2:2.0-1+deb11u2 -y
ENTRYPOINT ["/bin/ping"] 
CMD ["-V"]
