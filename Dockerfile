FROM debian:stretch-slim

LABEL maintainer="Alexandr Topilski <support@fastogt.com>"

ENV USER fastocloud
ENV APP_NAME fastocloud
ENV PROJECT_DIR /usr/src/fastocloud
ENV BUILD_DEPS='ca-certificates git python3 python3-pip nano dbus g++ cmake build-essential python3-setuptools autotools-dev curl wget vim iproute2 flex libssl-dev libproxy-dev yasm nasm ninja-build libexpat1-dev libpython3-dev libpython3.5 libpython3.5-dev python3.5-dev autoconf file libmagic-mgc libmagic1 libglib2.0-0 gettext-base libcroco3 libbison-dev libudev-dev libblkid-dev uuid-dev dconf-gsettings-backend dconf-service glib-networking-common glib-networking-services gsettings-desktop-schemas libdconf libpciaccess-dev libxfixes3 x11proto-fixes-dev'
ENV PREFIX=/usr/local 
ENV PYFASTOGT_DIR=/usr/src/pyfastogt
ENV PKG_CONFIG_PATH=${PKG_CONFIG_PATH}:/usr/local/lib/pkgconfig
ENV PATH=${PATH}:/usr/local/lib/pkgconfig

COPY . $PROJECT_DIR
COPY docker/fastocloud.conf /etc/

RUN groupadd -r $USER && useradd -r -g $USER $USER \
  && apt-get -qq update \
  && apt-get -qq install -y $BUILD_DEPS --no-install-recommends \
  && pip3 install setuptools \
  && mkdir -p $PYFASTOGT_DIR \
  && git clone https://github.com/fastogt/pyfastogt $PYFASTOGT_DIR \
  && git submodule update --init --recursive \
  && cd $PYFASTOGT_DIR \
  && python3 setup.py install  \
  && cd $PROJECT_DIR/build \
  && git submodule update --init --recursive \
  && ./env/build_env.py --prefix=$PREFIX --docker \
  && cd $PROJECT_DIR/build \
  && python3 build.py release $PREFIX \
  && rm -rf $PYFASTOGT_DIR $PROJECT_DIR \
  # && apt-get purge -y --auto-remove $BUILD_DEPS \
  && mkdir /var/run/$APP_NAME \
  && chown $USER:$USER /var/run/$APP_NAME


VOLUME /var/run/$APP_NAME
WORKDIR /var/run/$APP_NAME

ENTRYPOINT ["fastocloud"]

EXPOSE 6317 8000 7000 6001
