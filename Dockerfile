FROM ubuntu:22.04


RUN apt-get update \
    && apt-get upgrade 

RUN apt-get install -y build-essential python3 python3-pip cmake

RUN python3 -m pip install conan

ARG USER
ARG USER_ID
ARG GROUP_ID

# Support running as current user
# https://jtreminio.com/blog/running-docker-containers-as-current-host-user/#ok-so-what-actually-works
RUN groupadd -g ${GROUP_ID} ${USER} \
    && useradd -l -u ${USER_ID} -g ${GROUP_ID} ${USER} \
    && install -d -m 0755 -o ${USER} -g ${USER} /home/${USER}

WORKDIR /home/${USER}
USER ${USER}
