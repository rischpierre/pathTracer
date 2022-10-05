cd /tmp
USD_VERSION=19.11
USD_INSTALL_ROOT=/opt/USD
git clone -b v${USD_VERSION} https://github.com/PixarAnimationStudios/USD
python /tmp/USD/build_scripts/build_usd.py  \
    --no-embree \
    --no-maya \
    --no-houdini  \
    --no-imaging  \
    --no-python \
    --no-ptex \
    --no-usdview \
    --no-prman \
    --no-docs \
    --no-tests \
    --no-draco \
    ${USD_INSTALL_ROOT}
rm -rf /tmp/USD